#!/bin/bash -e

set -e
scriptdir="$( cd "$(dirname "$0")" ; pwd -P )"

shopt -s nocasematch
case "$(uname -a)" in
*darwin*)
    OS="mac"
    ;;
*linux*)
    OS="linux"
    ;;
cygwin*|mingw32*|msys*|mingw*)
    OS="windows"
    ;;
*)
    OS="unknown"
    ;;
esac

rm -f valgrind.txt
valgrind --leak-check=full --log-file=valgrind.txt -s ./ssem_main