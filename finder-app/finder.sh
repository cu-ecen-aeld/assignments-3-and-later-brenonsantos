#!/bin/sh


# Path to a directory on the filesystem
filesdir=$1
# Text string that will be searched whithin these files
searchstr=$2


# Exits with return value 1 error and print statements if any of the parameters above were not specified
if [ -z "$filesdir" ]
then
    echo "No directory specified"
    echo "Usage: finder.sh <directory> <search-string>"
    exit 1
fi

if [ -z "$searchstr" ]
then
    echo "No search string specified"
    echo "Usage: finder.sh <directory> <search-string>"
    exit 1
fi

# Exits with return value 1 error and print statements if filesdir does not represent a directory on the filesystem
if [ ! -d "$filesdir" ]
then
    echo "Directory $filesdir does not exist"
    exit 1
fi

# ----------- From assignment -------------
# Prints a message "The number of files are X and the number of matching lines are Y" 
# where X is the number of files in the directory and all subdirectories
# Y is the number of matching lines found in respective files, where a matching line refers to a line which contains searchstr (and may also contain additional content)
# -----------------------------------------

# Number of files in the directory and all subdirectories
numfiles=$(find $filesdir -type f | wc -l)
# find $filedir will search files and directories in the filedir directory; -type f will search for files only; 
# wc -l will count the number of lines in the output of find $filedir -type f

# Number of matching lines found in respective files
nummatches=$(grep -r "$searchstr" $filesdir | wc -l)
# grep -r "$searchstr" $filesdir will search for the string searchstr in all files in the directory and all subdirectories recursively;
# wc -l will count the number of lines in the output of grep -r "$searchstr" $filesdir

echo "The number of files are $numfiles and the number of matching lines are $nummatches"