# SupercellFlash

Supercell Flash is a C++ library that allows you to quickly read, modify and save .sc files

## Example

```
#include <SupercellFlash.h>
#include <iostream>

using namespace std;
using namespace sc;

int main(int argc, char* argv[]) {
    if (argc != 0) {
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
So, Premake5 is used here as a configuration system. Make sure you have but installed and you can use it. Then use the following command:
```
premake5 [action] --file=Workspace.lua
```

You can view the list of "actions" [here](https://premake.github.io/docs/Using-Premake). 
Then, depending on your system and compiler, you should be able to successfully build project.
