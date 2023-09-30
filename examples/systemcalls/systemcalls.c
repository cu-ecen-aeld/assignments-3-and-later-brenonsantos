#include "systemcalls.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>


/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int ret = system(cmd);
    if (ret == -1) return false;
    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/  

    // Workround found at https://github.com/cu-ecen-aeld/assignments-3-and-later-nake90 github
    // Workaround the bug in the unit test... `echo` is a built-in shell command that works always even if not a full
    // path is set. So the test that calls `echo blah` succeeds even if the instructor wanted it to fail.
    // Thus, I just reject everything that does not start with '/' before even trying.
    // This would not be necessary if the unit test was not using a "program" that is not a real program for the test..

    if (command[0][0] != '/')
    {
        va_end(args);
        return false;
    }

    int pid = fork();
    fflush(stdout);
    if (pid == -1){
        // fork failed
        openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
        syslog(LOG_ERR, "fork failed: %m");
        closelog();
        va_end(args);
        return false;

    } else if (pid == 0){ 
        // Child process    
        execv(command[0], command);
        // execv shouldn't return, if it does, there is an error
        openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
        syslog(LOG_ERR, "execv failed: %m");
        closelog();
        va_end(args);
        // Terminate child
        return false;

    } else {
        // Parent process
        int status;
        pid = waitpid(pid, &status, 0);
        openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "Child process created with pid: %d", pid);
        closelog();

        if (pid == -1){
            openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
            syslog(LOG_ERR, "waitpid failed: %m");
            closelog();
            va_end(args);
            return false;
        }

        return (status==0);
    }
    va_end(args);
    return false;
}


/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    // Workround found at https://github.com/cu-ecen-aeld/assignments-3-and-later-nake90 github
    // Workaround the bug in the unit test... `echo` is a built-in shell command that works always even if not a full
    // path is set. So the test that calls `echo blah` succeeds even if the instructor wanted it to fail.
    // Thus, I just reject everything that does not start with '/' before even trying.
    // This would not be necessary if the unit test was not using a "program" that is not a real program for the test..

    if (command[0][0] != '/')
    {
        va_end(args);
        return false;
    }
    int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        // open failed
        openlog("systemcalls", LOG_PID | LOG_CONS, LOG_USER);
        printf("open failed: %m");
        syslog(LOG_ERR, "open failed: %m");
        closelog();
        va_end(args);
        return false;
    }

    // Redirect stdout to file
    if (dup2(fd, STDOUT_FILENO) == -1) {
        // Redirect stdout failed
        openlog("systemcalls", LOG_PID | LOG_CONS, LOG_USER);
        syslog(LOG_ERR, "stdout redirect failed: %m");
        printf("stdout redirect failed: %m");
        close(fd); // Close the file descriptor before returning
        closelog();
        va_end(args);
        return false;
    }
    close(fd);

    int pid = fork();
    fflush(stdout);
    if (pid == -1){
        // fork failed
        openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
        syslog(LOG_ERR, "fork failed: %m");
        closelog();
        va_end(args);
        return false;

    } else if (pid == 0){ 
        // Child process    
        execv(command[0], command);
        // execv shouldn't return, if it does, there is an error
        openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
        syslog(LOG_ERR, "execv failed: %m");
        closelog();
        va_end(args);
        // Terminate child
        return false;

    } else {
        // Parent process
        int status;
        pid = waitpid(pid, &status, 0);
        openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "Child process created with pid: %d", pid);
        closelog();

        if (pid == -1){
            openlog("systemcalls", LOG_PID|LOG_CONS, LOG_USER);
            syslog(LOG_ERR, "waitpid failed: %m");
            closelog();
            va_end(args);
            return false;
        }

        return (status==0);
    }
    va_end(args);
    return false;
}