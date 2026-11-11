# scripts/idf-env.sh -- put an ESP-IDF toolchain on PATH for a NON-interactive
# shell.  Source this; do not run it.
#
# The interactive way in is the alias `useidf-6-1`, which sources
# ~/.espressif/tools/activate_idf_v6.1.sh.  That script refuses to run from a
# shell script (it checks $0 and exits), and the `idf.py` it provides is a shell
# alias, which a non-interactive shell will not expand.  So instead we ask it
# for its environment with `-e` and export the parts we need ourselves, then
# call tools/idf.py through the venv's python directly.
#
# The project is built with ESP-IDF 6.1 (m5gfx/m5unified 0.2.36/0.2.29 or later;
# earlier m5gfx releases include driver/i2s.h, which IDF 6 removed).  Override
# the version with IDF_VERSION=x.y ./build.sh, or point
# IDF_ACTIVATE at an activation script anywhere.

IDF_VERSION="${IDF_VERSION:-6.1}"
IDF_ACTIVATE="${IDF_ACTIVATE:-$HOME/.espressif/tools/activate_idf_v${IDF_VERSION}.sh}"

if [ ! -f "$IDF_ACTIVATE" ]; then
    echo "error: no IDF activation script at $IDF_ACTIVATE" >&2
    echo "       set IDF_VERSION or IDF_ACTIVATE; available:" >&2
    ls "$HOME/.espressif/tools/" 2>/dev/null | grep '^activate_idf' >&2
    return 1 2>/dev/null || exit 1
fi

while IFS= read -r _line; do
    _key="${_line%%=*}"
    _val="${_line#*=}"
    case "$_key" in
        IDF_PATH|IDF_TOOLS_PATH|IDF_PYTHON_ENV_PATH|ESP_ROM_ELF_DIR| \
        OPENOCD_SCRIPTS|IDF_COMPONENT_LOCAL_STORAGE_URL|ESP_IDF_VERSION)
            export "$_key=$_val"
            ;;
        PATH)
            PATH="$_val:$PATH"
            export PATH
            ;;
    esac
done <<EOT
$(sh "$IDF_ACTIVATE" -e)
EOT
unset _line _key _val

if [ -z "${IDF_PATH:-}" ]; then
    echo "error: $IDF_ACTIVATE did not report an IDF_PATH" >&2
    return 1 2>/dev/null || exit 1
fi

IDF_PYTHON="${IDF_PYTHON_ENV_PATH:-}/bin/python"
if [ ! -x "$IDF_PYTHON" ]; then
    IDF_PYTHON="$(command -v python3 || command -v python)"
fi
export IDF_PYTHON

# Build directory for the command-line scripts.  It must be passed to idf.py
# explicitly: IDF 6.1's idf.py otherwise takes the build directory from the
# first preset in CMakePresets.json, which belongs to CLion.
IDF_BUILD_DIR="${IDF_BUILD_DIR:-build}"

# idf_py <args...> -- run idf.py from the activated IDF, in $IDF_BUILD_DIR
idf_py()
{
    "$IDF_PYTHON" "$IDF_PATH/tools/idf.py" -B "$IDF_BUILD_DIR" "$@"
}

# pick_port <glob> -- first matching serial device, or empty
pick_port()
{
    for _p in $1; do
        [ -e "$_p" ] && { printf '%s\n' "$_p"; return 0; }
    done
    return 1
}
