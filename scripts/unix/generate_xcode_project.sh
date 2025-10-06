#!/bin/bash

# Set dirs
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="${SCRIPT_DIR}/../.."
BUILD_DIR="${REPO_ROOT}/build"

# Remake build
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Generate Xcode project using CMake
cmake "$REPO_ROOT" -G "Xcode" -DSC_FLASH_BUILD_TOOLS=ON --fresh
