@echo off
cd ../
call premake5 vs2019 --file=Workspace.lua
popd
pause
