#!/usr/bin/env bash
# flash.sh -- bring a build directory up to date and flash it to the Tab5.
#
#   ./flash.sh                                   flash build/, autodetect port
#   ./flash.sh -B cmake-build-debug-esp-idf-6.1  flash CLion's build directory
#   ./flash.sh -p /dev/cu.usbmodem101            use a specific port
#   ./flash.sh --no-build                        flash what is already built
#
# It first builds the directory's default target (the ELF alone is not enough:
# Tab5SSEM.bin, the bootloader and the partition table are separate targets),
# then writes exactly the images listed in <build dir>/flash_args.  So the
# image on the board always matches the ELF the debugger loads symbols from.
# CLion's "Tab5 Debug (OpenOCD)" runs this against CLion's build directory.
#
# The board has two USB paths and they are NOT interchangeable:
#
#   /dev/cu.usbmodem*    the P4's USB-Serial/JTAG port -- esptool and OpenOCD
#   /dev/cu.usbserial-*  the external bridge on UART0 -- the console
#
# esptool and OpenOCD need the same USB-Serial/JTAG device, so stop any running
# OpenOCD (CLion debug session) before flashing.  The console stays usable.

set -euo pipefail
cd "$(dirname "$0")"
# shellcheck source=scripts/idf-env.sh
. scripts/idf-env.sh

BUILD_DIR=build
PORT="${ESPPORT:-}"
BUILD=true
while [ $# -gt 0 ]; do
    case "$1" in
        -B) BUILD_DIR="$2"; shift 2 ;;
        -p) PORT="$2"; shift 2 ;;
        --no-build) BUILD=false; shift ;;
        -h|--help) sed -n '2,9p' "$0"; exit 0 ;;
        *) echo "error: unknown option $1" >&2; exit 1 ;;
    esac
done

if $BUILD; then
    if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
        echo "error: $BUILD_DIR is not configured; run ./build.sh or load CMake in CLion" >&2
        exit 1
    fi
    echo "==> building $BUILD_DIR"
    cmake --build "$BUILD_DIR"
fi

if [ ! -f "$BUILD_DIR/flash_args" ]; then
    echo "error: $BUILD_DIR/flash_args not found; build the project first" >&2
    exit 1
fi

if [ -z "$PORT" ]; then
    PORT="$(pick_port '/dev/cu.usbmodem*' || true)"
fi
if [ -z "$PORT" ]; then
    echo "error: no /dev/cu.usbmodem* found; pass the port with -p" >&2
    ls /dev/cu.* 2>/dev/null >&2 || true
    exit 1
fi

if lsof -nP -iTCP:3333 -sTCP:LISTEN >/dev/null 2>&1; then
    echo "error: OpenOCD is running (port 3333 is in use); stop the debug session first" >&2
    exit 1
fi

echo "==> flashing $BUILD_DIR via $PORT"
cd "$BUILD_DIR"
"$IDF_PYTHON" -m esptool --chip esp32p4 -p "$PORT" -b 460800 \
    --before default-reset --after hard-reset write-flash @flash_args
