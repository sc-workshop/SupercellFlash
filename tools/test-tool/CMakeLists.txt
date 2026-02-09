# Project: supercell-flash test-tool

# C++ headers and source files
set(SOURCES source/main.cpp)

set(CLI_TARGET supercell-flash-test-tool)

add_executable(${CLI_TARGET} ${SOURCES})

set_target_properties(${CLI_TARGET} PROPERTIES
    FOLDER supercell-sdk/cli
)

target_link_libraries(${CLI_TARGET} PRIVATE supercell::flash)
wk_project_setup(${CLI_TARGET})

install(
    TARGETS ${CLI_TARGET}
    RUNTIME DESTINATION bin
)