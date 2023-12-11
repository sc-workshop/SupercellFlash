@echo off
cd ../
call make config=%1_armv7a CFLAGS=-fPIC CXXFLAGS=-fPIC CXX=armv7a-linux-androideabi22-clang++ CC=armv7a-linux-androideabi22-clang   %2
cd scripts
popd
pause