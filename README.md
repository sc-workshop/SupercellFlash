# SupercellFlash

Supercell Flash is a C++ library that allows you to quickly read, modify and save .sc files

## Example

```
#include <SupercellFlash.h>
#include <iostream>

using namespace std;
using namespace sc;

int main(int argc, char* argv[]) {
    if (argc != 1) {
        cout << "Failed to get filename";
    }

    SupercellSWF swf;

    try {
        swf.load(argv[0]);
    }
    catch (const std::exception& err) {
        cout << "Failed to load file. Error: " << err.what() << endl;
    }

    cout << "File has: " << endl;
    cout << swf.exports.size() << " export names" << endl;
    cout << swf.textures.size() << " textures" << endl;
    cout << swf.textFields.size() << " textfields" << endl;
    cout << swf.shapes.size() << " shapes" << endl;
    cout << swf.movieClips.size() << " movieclips" << endl;

    return 0;
}
```

## Building
Before starting, install all necessary submodules
```
git update --remote
```

CMake build system is used. Make sure you have but installed and you can use it. Then just run ```cmake``` command from project root directory with specified generator type and output directory.
Example for Visual Studio 2019 for Windows (also you can use 2022 version):
```
cmake -G "Visual Studio 16 2019" -B ./build
```
