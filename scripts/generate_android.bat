@echo off
cd ../
call premake5 gmake --file=Workspace.lua --os=android
popd
pause
