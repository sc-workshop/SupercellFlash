cmake_minimum_required(VERSION 3.22)

# workspace
project(supercell-sdk)

# options
option(SC_FLASH_BUILD_TOOLS "Build tools of supercell::flash library" OFF)

# includes
include(FetchContent)
include(cmake/dependencies.cmake)
include(WkFlatbuffers)

# projects
add_subdirectory(supercell-flash)

if(SC_FLASH_BUILD_TOOLS)
    include(WkJson)
    add_subdirectory(tools/test-tool)
    add_subdirectory(tools/texture-tool)
endif()
