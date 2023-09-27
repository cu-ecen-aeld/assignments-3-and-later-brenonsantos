#!/bin/sh

# ------------- From assignment -----------------
# Write a shell script finder-app/writer.sh as described below
#  - Accepts the following arguments: the first argument is a full path to a file (including filename) on the filesystem, 
#   referred to below as writefile; the second argument is a text string which will be written within this file, referred to below as writestr
#
#  - Exits with value 1 error and print statements if any of the arguments above were not specified
#   
#  - Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating
#   the path if it doesn’t exist. Exits with value 1 and error print statement if the file could not be created.
# ------------------------------------------------

# Path to a file on the filesystem
writefile=$1
# Text string that will be written within this file
writestr=$2

# Exits with return value 1 error and print statements if any of the parameters above were not specified
if [ -z "$writefile" ]
then
    echo "No file specified"
    echo "Usage: writer.sh <file> <write-string>"
    exit 1
fi

if [ -z "$writestr" ]
then
    echo "No write string specified"
    echo "Usage: writer.sh <file> <write-string>"
    exit 1
fi

# Extract the directory path from the specified file
writefile_dir=$(dirname "$writefile")

# Create the directory if it doesn't exist
if [ ! -d "$writefile_dir" ]
then
    mkdir -p "$writefile_dir"
fi


# Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating the path if it doesn’t exist
echo "$writestr" > $writefile

# Exits with value 1 and error print statement if the file could not be created
if [ ! -f "$writefile" ]
then
    echo "File $writefile could not be created"
    exit 1
fi

