# Desktop Specific Files

In this folder you will find the files that will allow the SSEM emulator to be run on a desktop machine as well as perform some source code checks.

The makefile will draw the core system files from the NuttX directory as well as the files that are specific to running the application on a desktop.  For instance, the registration of the file system on the desktop differs from the registration process in NuttX.

## Dockerfile

A Dockerfile has been provided to supply the standard desktop development tools found in Linux systems to allow the code to be built under Ubuntu Linux.  This also provides access to the valgrind tool for memory checking.

## runvalgrind.sh

This script runs the application using valgrind producing a summary report in the file valgrind.txt.
