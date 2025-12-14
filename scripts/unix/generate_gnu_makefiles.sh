#!/bin/bash

# Set dirs
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="${SCRIPT_DIR}/../.."
BUILD_DIR="${REPO_ROOT}/build"

# Remake build
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Generate Makefiles using CMake and build
cmake "$REPO_ROOT" -G "Unix Makefiles" -DSC_FLASH_BUILD_TOOLS=ON --fresh
make supercell-flash-texture-tool
