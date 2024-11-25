cmake_minimum_required(VERSION 3.22)

# workspace
project(supercell-sdk)

# options
option(SC_FLASH_BUILD_TOOLS "Build tools of supercell::flash library" ON)

# includes
include(FetchContent)

if(SC_FLASH_BUILD_TOOLS)
    set(WK_CLI_DEPENDENCIES ON)
    set(WK_SERIALIZATION_JSON ON)
endif()

set(WK_SERIALIZATION_FLATBUFFER ON)

include(cmake/dependencies.cmake)

# projects
add_subdirectory(supercell-flash)

if(SC_FLASH_BUILD_TOOLS)
    add_subdirectory(tools/test-tool)
    add_subdirectory(tools/texture-tool)
endif()
