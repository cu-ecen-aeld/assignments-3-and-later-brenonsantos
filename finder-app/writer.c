/* Linux System Programming and Introduction to Buildroot - Assignment 2
*  Author: Breno Niehues dos Santos
*  Date: 20/03/2021
*
*  Assignment 2:
    - Write a C application “writer” (finder-app/writer.c)  which can be used as an alternative to the
      “writer.sh” test script created in assignment1 and using File IO as described in LSP chapter 2. 
      See the Assignment 1 requirements for the writer.sh test script and these additional instructions:
            One difference from the write.sh instructions in Assignment 1:  You do not need to make your
            "writer" utility create directories which do not exist.  You can assume the directory is
            created by the caller.

            Setup syslog logging for your utility using the LOG_USER facility.

            Use the syslog capability to write a message “Writing <string> to <file>” where <string> is
            the text string written to file (second argument) and <file> is the file created by the script.
            This should be written with LOG_DEBUG level.

            Use the syslog capability to log any unexpected errors with LOG_ERR level.

*  Assignment 1:
    - Write a shell script finder-app/writer.sh as described below
            Accepts the following arguments: the first argument is a full path to a file (including filename) 
            on the filesystem, referred to below as writefile; the second argument is a text string which will
            be written within this file, referred to below as writestr

            Exits with value 1 error and print statements if any of the arguments above were not specified

            Creates a new file with name and path writefile with content writestr, overwriting any existing
            file and creating the path if it doesn’t exist. Exits with value 1 and error print statement if
            the file could not be created.
*/


#include <stdio.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char *argv[]){
    // Check if the correct number of command-line arguments are provided
    if (argc != 3) {
        printf("Incorrect arguments provided.\n");
        printf("Usage: %s <file_path> <text_string>\n", argv[0]);
        return 1; // Exit with an error code
    }
    // printf("Writer utility started.\n");
    // Get the file path and text from command-line arguments
    char *writefile = argv[1];
    char *writestr = argv[2];
    openlog("Writer", LOG_PID, LOG_USER);
    
    int fd;
    ssize_t nr;
    ssize_t count = strlen(writestr);

    if ((fd = creat(writefile, 0655)) < 0){
        syslog(LOG_ERR, "Error creating file %s", writefile);
        // printf("Error creating file %s\n", writefile);
        return 1;
    }

    nr = write(fd, writestr, count);
    if (nr < 0){
        syslog(LOG_ERR, "Error writing to file %s", writefile);
        // printf("Error writing to file %s\n", writefile);
        return 1;
    } else if (nr != count){
        syslog(LOG_ERR, "Error writing to file %s", writefile);
        // printf("Error writing to file %s\n", writefile);
        return 1;
    }
    syslog(LOG_DEBUG, "Writing \"%s\" to \"%s\"", writestr, writefile);
    // printf("Writing \"%s\" to \"%s\"\n", writestr, writefile);
    
    if (close(fd) < 0){
        syslog(LOG_ERR, "Error closing file %s", writefile);
        // printf("Error closing file %s\n", writefile);
        return 1;
    }
    closelog();
    return 0;

}