@echo off
cd ../
call make config=%1 CFLAGS=-fPIC CXXFLAGS=-fPIC CXX=aarch64-apple-darwin22.3.0-clang++ CC=aarch64-apple-darwin22.3.0-clang  %2
cd scripts
popd
pause