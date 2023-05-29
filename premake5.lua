

workspace "SupercellSWF"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

group "Compression"
	include "external/lzma"
    include "external/lzham"
    include "external/zstd"

group "Libraries" 
	include "SupercellBytestream"
	include "SupercellCompression"
	include "SupercellFlash"

group "Tests"
	include "Tests/SupercellFlash"
	include "Tests/SupercellCompression"
