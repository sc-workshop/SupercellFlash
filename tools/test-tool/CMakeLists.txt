# Project: supercell-flash test-tool

# C++ headers and source files
set(SOURCES main.cpp)

set(CLI_TARGET supercell-flash-test-tool)

add_executable(${CLI_TARGET} ${SOURCES})

set_target_properties(${CLI_TARGET} PROPERTIES
    FOLDER SupercellSDK/tools
)

target_link_libraries(${CLI_TARGET} PRIVATE supercell::flash)
