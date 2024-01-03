include(FetchContent)

FetchContent_Declare(
    SupercellCompression
    GIT_REPOSITORY https://github.com/sc-workshop/SupercellCompression
    GIT_TAG dev
)
FetchContent_Declare(
    SupercellCore
    GIT_REPOSITORY https://github.com/sc-workshop/SC-Core
    GIT_TAG main
)

FetchContent_MakeAvailable(SupercellCore)
FetchContent_MakeAvailable(SupercellCompression)

set(TARGET "SupercellFlash")

set(SOURCES
    "source/SupercellSWF.cpp"

    "source/objects/ColorTransform.cpp"
    "source/objects/Matrix2D.cpp"
    "source/objects/MatrixBank.cpp"

    "source/objects/MovieClip.cpp"
    "source/objects/MovieClipFrame.cpp"
    "source/objects/MovieClipModifier.cpp"

    "source/objects/Shape.cpp"
    "source/objects/ShapeDrawBitmapCommand.cpp"

    "source/objects/SWFTexture.cpp"

    "source/objects/TextField.cpp"
)

set(HEADERS
    "include/SupercellFlash.h"

    "include/SupercellFlash/SupercellSWF.h"
    "include/SupercellFlash/Tags.h"

    "include/SupercellFlash/exception/NegativeTagLengthException.h"
    "include/SupercellFlash/exception/ObjectLoadingException.h"

    "include/SupercellFlash/objects/DisplayObject.h"

    "include/SupercellFlash/objects/ColorTransform.h"
    "include/SupercellFlash/objects/Matrix2D.h"
    "include/SupercellFlash/objects/MatrixBank.h"

    "include/SupercellFlash/objects/ExportName.h"
    "include/SupercellFlash/objects/MovieClip.h"
    "include/SupercellFlash/objects/MovieClipFrame.h"
    "include/SupercellFlash/objects/MovieClipModifier.h"

    "include/SupercellFlash/objects/Shape.h"
    "include/SupercellFlash/objects/ShapeDrawBitmapCommand.h"

    "include/SupercellFlash/objects/SWFTexture.h"

    "include/SupercellFlash/objects/TextField.h"
)

add_library(${TARGET} STATIC ${SOURCES} ${HEADERS})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})
set_target_properties(${TARGET} PROPERTIES
  FOLDER Supercell
)
sc_core_base_setup(${TARGET})

target_link_libraries(${TARGET} PUBLIC 
    SupercellCompression
)

target_include_directories(${TARGET}
  PUBLIC
  "include/"
)

