cmake_minimum_required(VERSION 3.22)

project(SupercellFlash)

include(cmake/SupercellFlash.cmake)

option(SC_FLASH_TEST "Build SC Flash Test" OFF)
option(SC_FLASH_TEXTURE_TOOL "Build SC Flash Texture Tool" OFF)

if(${SC_FLASH_TEST})
    include(cmake/SupercellFlashTest.cmake)
endif()

if(${SC_FLASH_TEXTURE_TOOL})
    include(cmake/SupercellFlashTextureTool.cmake)
endif()