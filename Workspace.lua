workspace "ScFlash"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    include "./"
    include "dependencies/TextureLoader"

	include "./Test"

    group "Textures"
        include "dependencies/TextureLoader/ThirdParty/libktx"
        include "dependencies/TextureLoader/ThirdParty/dfdutils"

    group "Compression"
		include "dependencies/Compression"
        include "dependencies/Compression/dependencies/lzma"
        include "dependencies/Compression/dependencies/lzham"
        include "dependencies/Compression/dependencies/zstd"

    group "Compression/Image"
        include "dependencies/TextureLoader/ThirdParty/ETCPACK"
        include "dependencies/TextureLoader/ThirdParty/astc-encoder"
        include "dependencies/TextureLoader/ThirdParty/basisu"