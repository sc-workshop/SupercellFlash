# SupercellFlash

Supercell Flash is a C++ library that allows you to quickly read, modify and save .sc files

### Currently supports:
- Full support for SC1
- Support for SC2 V5, partial V6 support 

### TODO: 
- Support for ScCompressedExternalMatrixBank

## Building

CMake build system is used. Make sure you have but installed and you can use it. Then just run ```cmake``` command from project root directory with specified generator type and output directory.
Example for Visual Studio 2019 for Windows (also you can use 2022 version):
```
cmake -G "Visual Studio 16 2019" -B ./build
```
