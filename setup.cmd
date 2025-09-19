@echo off


rem ## Check CMake is installed
cmake --version
if %errorlevel% neq 0 (
    echo CMake is not installed!
    echo Go to: https://cmake.org/download/
    goto End
)


rem ## Check FlatBuffers is installed
flatc --version
if %errorlevel% neq 0 (
    echo FlatBuffers is not installed!
    echo Go to: https://github.com/google/flatbuffers/releases/
    echo Download the latest version of the binary file and add it to the PATH system variable!
    goto End
)


rem ## Call CMake and generate Visual Studio solution
call .\scripts\windows\generate_visual_studio_solution.cmd
goto End

:End
pause
