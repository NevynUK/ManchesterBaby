#!/bin/bash

set -e
scriptdir="$( cd "$(dirname "$0")" ; pwd -P )"

#
#   Work out the OS so that we can change actions per OS where necessary.
#
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

#
# Check if the shell is interactive.
#
if [[ $- == *i* ]]; then
  red=`tput setaf 1`
  green=`tput setaf 2`
  reset=`tput sgr0`
fi

VERBOSE=true
CLEAN=false
FULLCLEAN=false
HELP=false
DRYRUN=false
IDF_FLAGS=""
IDF_DEFINES=""
FLASH=false
BUILD=false

#
#   Work out which options are on the command line.
#
for i in "$@"
do
case $i in
    -v|--verbose)
    VERBOSE=true
    ;;
    -b|--build)
    BUILD=true
    IDF_FLAGS+=" build"
    ;;
    -c|--clean)
    IDF_FLAGS+=" clean"
    ;;
    --fullclean)
    FULLCLEAN=true
    IDF_FLAGS+=" fullclean"
    ;;
    --debug)
    DEBUG=true
    ;;
    --dryrun)
    DRYRUN=true
    ;;
    -f|--flash)
    IDF_FLAGS+=" flash"
    FLASH=true
    ;;
    -m|--monitor)
    IDF_FLAGS+=" monitor"
    ;;
    -h|--help)
    HELP=true
    ;;
    *)
    printf "Unknown command line option ($i)."
    printf "Run build.sh --help for information on valid options."
    exit 1
    ;;
esac
done

if [[ $# -eq 0 ]]; then
    # No arguments were passed
    echo "No arguments supplied, performing default build."
    BUILD=true
    IDF_FLAGS+=" build"
fi

if $HELP; then
    echo "$0: Build the ESP code for Meadow"
    echo ""
    echo "Default action: Build the ESP application in release mode."
    echo ""
    echo "Options:"
    echo "-h | --help             Display this message"
    echo "-c | --clean            Perform a clean build"
    echo "-f | --flash            Flash the ESP32 with the bootloader, partition table and application code"
    echo "-m | --monitor          Connect to the serial port for application monitoring"
    echo "--fullclean             Perform a full clean followed by a build"
    echo "-v | --verbose          Display commands as they are executed (default)"
    echo "-d=* | --debug=*        Turn on debug for selected system(s). Valid values (comma separated): WIFI, SPI, BT, SYSTEM."
    echo ""
    exit 0
fi

run_command() {
  if $VERBOSE || $DRYRUN; then
    echo "Executing command: $1"
    if ! $DRYRUN; then
        $1
    fi
  else
    if ! $DRYRUN; then
        $1 &>/dev/null
    fi
  fi
}

check_command_status() {
  exit_status=$?
  if [ $exit_status -ne 0 ]; then
    printf " ${red}error${reset}\n"
    if ! $VERBOSE; then
        printf "Re-run the script with --verbose flag to see the output.\n"
    fi
    exit 1
  else
    printf " ${green}success${reset}\n"
  fi
}

#
# Set the IDF variable in case we are running in a shell where eim has been used to install ESP-IDF.
#
IDF="$IDF_PATH/tools/idf.py"

#
#   There are sometimes problems with old configuration in the build directory
#   so this removes the build directory before executing the full clean.
#
if $FULLCLEAN; then
    run_command "rm -Rf $scriptdir/build"
    run_command " $IDF fullclean"
    run_command "rm -f $scriptdir/sdkconfig"
fi

if ! $NO_BUILD; then
    IDF_FLAGS="build $IDF_FLAGS"
fi
if [[ ! -z $IDF_FLAGS ]]; then
    $IDF $IDF_DEFINES $IDF_FLAGS
fi

now=$(date +"%T")
printf "Build finished at $now\n"
