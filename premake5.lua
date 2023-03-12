

workspace "SupercellTools"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    -- startproject "SupercellFlash"
	-- startproject "SupercellFlashTests"

group "Compression"
	include "external/lzma"
    include "external/lzham"
    include "external/zstd"

group "Libraries" 
	include "SupercellBytestream"
	include "SupercellCompression"

group "Tests"
	include "Tests/SupercellFlash"
	include "Tests/SupercellCompression"
