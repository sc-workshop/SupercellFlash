set(TARGET "SupercellFlashTextureTool")

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(${TARGET}
    "texture_tool/main.cpp"
    "texture_tool/SWFFile.hpp"
)

sc_core_base_setup(${TARGET})
target_link_libraries(${TARGET} PUBLIC SupercellFlash)
set_target_properties(${TARGET} PROPERTIES
    FOLDER Supercell/CLI
    OUTPUT_NAME "SCTex"
)

target_link_libraries(${TARGET} PRIVATE nlohmann_json::nlohmann_json)
