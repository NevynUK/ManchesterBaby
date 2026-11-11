# CLion tool wrappers

The CLion project files in `.idea/` and `CMakePresets.json` are committed, so
they must not contain a path that only exists on one machine. They name tools
by indirection instead:

| What | Committed reference | Resolved on each machine by |
|---|---|---|
| CMake profile | `CMakePresets.json` preset `Debug-ESP-IDF 6.1` | the CLion toolchain named below |
| Compiler, CMake environment | toolchain name `ESP-IDF 6.1` | CLion → Settings → Build → Toolchains |
| OpenOCD (debug server) | `$PROJECT_DIR$/scripts/clion/openocd` | `scripts/idf-env.sh` (IDF's `openocd-esp32`) |
| GDB | `$PROJECT_DIR$/scripts/clion/gdb` | `scripts/idf-env.sh` (IDF's `riscv32-esp-elf-gdb`) |
| Flash, monitor | `$PROJECT_DIR$/flash.sh`, `monitor.sh` | `scripts/idf-env.sh` |

## One-time setup per machine

1. Install ESP-IDF 6.1 with the Espressif installer (EIM), so that
   `~/.espressif/tools/activate_idf_v6.1.sh` exists.
2. In CLion, add a toolchain named exactly **ESP-IDF 6.1**: System toolset,
   with the environment file set to `~/.espressif/tools/activate_idf_v6.1.sh`.
3. Open the project. CLion loads the `Debug-ESP-IDF 6.1` profile from
   `CMakePresets.json`, and the Tab5 run configurations from
   `.idea/runConfigurations`.

## Overrides

`scripts/idf-env.sh` and the wrappers read these variables, either from the
login shell (CLion on macOS reads it at startup, so restart CLion after a
change) or from `scripts/clion/tools.local.env`, which is not committed (see
`tools.local.env.example`):

| Variable | Effect |
|---|---|
| `IDF_VERSION` | IDF to use, default `6.1` |
| `IDF_ACTIVATE` | full path to an IDF activation script |
| `OPENOCD` | specific OpenOCD binary |
| `IDF_GDB` | specific GDB binary |

## Troubleshooting

**Debug session ends at once with "Debugger exited with code 137 (SIGKILL)".**
This is rarely a GDB problem. CLion kills GDB when `-target-select remote
tcp:localhost:3333` times out, which happens when OpenOCD failed to start.
Check OpenOCD directly:

    scripts/clion/openocd -f debug.cfg -c "init; exit"

If it reports `esp_usb_jtag: IN buffer overflow` and `Unsupported DTM version:
-1`, the P4's USB-Serial/JTAG has wedged, typically after a debug session was
killed mid-transfer. A chip reset (esptool, the reset button) does not clear
it; unplug the USB-C cable and power-cycle the Tab5, then retry.

## Notes

Do not rely on an `openocd` that is already on `PATH`. Homebrew and distro
packages ship upstream OpenOCD, which does not support the ESP32-P4; the
wrapper always takes OpenOCD from the IDF environment unless `OPENOCD` is set.
