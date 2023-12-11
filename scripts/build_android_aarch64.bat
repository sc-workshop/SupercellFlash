@echo off
cd ../
call make config=%1_aarch64 CFLAGS=-fPIC CFLAGS:=-g CXXFLAGS=-fPIC CXXFLAGS:=-g CXX=aarch64-linux-android22-clang++ CC=aarch64-linux-android22-clang   %2
cd scripts
popd
pause