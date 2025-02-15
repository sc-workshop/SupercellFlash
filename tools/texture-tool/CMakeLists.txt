# Project: supercell-flash texture-tool

# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)

set(CLI_TARGET supercell-flash-texture-tool)

add_executable(${CLI_TARGET} ${HEADERS} ${SOURCES})
wk_project_setup(${CLI_TARGET})
wk_include_json()
wk_include_argparse()

set_target_properties(${CLI_TARGET} PROPERTIES
    FOLDER SupercellSDK/tools
)

target_include_directories(${CLI_TARGET} PUBLIC source)

target_link_libraries(${CLI_TARGET} PRIVATE supercell::flash)

set_target_properties(${CLI_TARGET} PROPERTIES
    FOLDER Supercell/CLI
    OUTPUT_NAME "SCTex"
)
