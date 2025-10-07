#!/bin/bash

# This script can be called from anywhere.

# Check CMake
if ! command -v cmake >/dev/null 2>&1; then
    echo "CMake is not installed!"
    exit 1
fi

# Check FlatBuffers
if ! command -v flatc >/dev/null 2>&1; then
    echo "FlatBuffers is not installed!"
    exit 1
fi

# Call GNU Makefiles build script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
"$SCRIPT_DIR/scripts/unix/generate_gnu_makefiles.sh"
read -p "Press Enter to exit..."