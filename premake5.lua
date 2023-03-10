

workspace "SupercellFlash"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    startproject "Test"

include "SupercellFlash"
include "Test"
