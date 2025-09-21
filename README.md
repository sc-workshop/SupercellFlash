# Supercell Flash

Supercell Flash is a C++ library that allows you to quickly read, modify and save *.sc files.

## Features:
- Full support for SC1
- Support for SC2 V5, partial V6 support 

## Building
### Requirements
- CMake
- FlatBuffers
- IDE (Visual Studio, XCode, etc.)

The CMake build system is used. Make sure that it is installed and you can use it. The FlatBuffers library is also needed to generate templates for some classes (install the latest version of the binary file and add it to the **PATH** system variable!).

To generate the project files, run ```setup.cmd``` file (or ```setup.sh```/```setup.command``` if you are on Linux or Mac OS, but it's not implemented right now). Open the generated project in a new folder and build it using the IDE you prefer.

## TODO: 
- Support for ScCompressedExternalMatrixBank.
- Linux and Mac OS support.
