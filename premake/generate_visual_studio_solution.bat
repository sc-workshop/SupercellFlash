@echo off
pushd %~dp0\..\
call premake\bin\premake5.exe vs2019
popd
pause
