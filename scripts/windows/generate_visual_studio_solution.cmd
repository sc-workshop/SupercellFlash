@echo off
color 0a

REM Set dirs
set SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%
set REPO_ROOT=%SCRIPT_DIR%\..\..
set BUILD_DIR=%REPO_ROOT%\build

REM Remake build directory
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM CMake build
cmake "%REPO_ROOT%" -G "Visual Studio 17 2022" -DSC_FLASH_BUILD_TOOLS=ON --fresh
cmake --build . --target supercell-flash-texture-tool --config Release
pause
