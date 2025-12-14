@echo off

REM This script can be called from anywhere now. Like from VS Code Developer Command Prompt.

REM Check CMake is installed
cmake --version
if %errorlevel% neq 0 (
    echo CMake is not installed!
    echo Go to: https://cmake.org/download/
    pause
)

REM Check FlatBuffers is installed
flatc --version
if %errorlevel% neq 0 (
    echo FlatBuffers is not installed!
    echo Go to: https://github.com/google/flatbuffers/releases/
    echo Download the latest version of the binary file and add it to the PATH system variable!
    pause
)

REM Call CMake and generate Visual Studio solution
call "%~dp0scripts\windows\generate_visual_studio_solution.cmd"
pause