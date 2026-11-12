#!/usr/bin/env bash
# debug.sh -- start OpenOCD on the Tab5's USB-Serial/JTAG port for GDB.
#
#   ./debug.sh            reset and halt the board, then wait for GDB
#   ./debug.sh --attach   attach to the running application without a reset
#
# Then, in another terminal:
#
#   . scripts/idf-env.sh
#   riscv32-esp-elf-gdb -x gdbinit build/Tab5SSEM.elf
#
# CLion's "Tab5 Debug (OpenOCD)" configuration starts OpenOCD the same way, so
# do not run this while a CLion debug session is active.

set -euo pipefail
cd "$(dirname "$0")"
# shellcheck source=scripts/idf-env.sh
. scripts/idf-env.sh

INIT="init; reset halt"
case "${1:-}" in
    --attach) INIT="init" ;;
    "") ;;
    *) echo "error: unknown option $1" >&2; exit 1 ;;
esac

exec openocd -s "$OPENOCD_SCRIPTS" -f debug.cfg -c "$INIT"
