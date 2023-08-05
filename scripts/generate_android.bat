@echo off
cd ../
call premake5 gmake --file=Workspace.lua --platform=android
popd
pause
