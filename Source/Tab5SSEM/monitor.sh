#!/usr/bin/env bash
# monitor.sh -- open the application console.
#
#   ./monitor.sh                                    UART0 bridge, build/ ELF
#   ./monitor.sh -B cmake-build-debug-esp-idf-6.1   decode with CLion's ELF
#   ./monitor.sh -p /dev/cu.usbserial-5B1F0073251
#
# The console is UART0, which on the Tab5 reaches the external USB bridge --
# /dev/cu.usbserial-*, NOT the /dev/cu.usbmodem* that flash.sh and OpenOCD
# use -- so it can stay open while the board is flashed and debugged.
# --no-reset keeps the monitor from resetting the board when it opens, which
# would otherwise disturb a running debug session.  Ctrl-] to quit.

set -euo pipefail
cd "$(dirname "$0")"
# shellcheck source=scripts/idf-env.sh
. scripts/idf-env.sh

BUILD_DIR=build
PORT="${ESPMONITORPORT:-}"
while [ $# -gt 0 ]; do
    case "$1" in
        -B) BUILD_DIR="$2"; shift 2 ;;
        -p) PORT="$2"; shift 2 ;;
        -h|--help) sed -n '2,6p' "$0"; exit 0 ;;
        *) echo "error: unknown option $1" >&2; exit 1 ;;
    esac
done

if [ -z "$PORT" ]; then
    PORT="$(pick_port '/dev/cu.usbserial-*' || true)"
fi
if [ -z "$PORT" ]; then
    echo "error: no /dev/cu.usbserial-* found; pass the port with -p" >&2
    ls /dev/cu.* 2>/dev/null >&2 || true
    exit 1
fi

ELF="$BUILD_DIR/Tab5SSEM.elf"
echo "==> console on $PORT, symbols from $ELF (Ctrl-] to quit)"
exec "$IDF_PYTHON" -m esp_idf_monitor -p "$PORT" -b 115200 --no-reset \
    --toolchain-prefix riscv32-esp-elf- --target esp32p4 "$ELF"
