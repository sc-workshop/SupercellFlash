
newoption {
    trigger = "platform",
    value = "OS",
    description = "Choose a platform to build library",
    allowed = {
       { "win",    "Windows" },
       { "macos",  "MacOSX" },
       { "android",  "Android" }
    },
    default = "win"
 }

workspace "ScFlash"
    configurations {
        "Debug",
        "Release"
    }

    filter {"options:platform=win"}
        system "windows"
        platforms { "Win32", "Win64" }

    filter {"options:platform=macos"}
        system "macosx"
        platforms { "x64" }

    filter {"options:platform=android"}
        system "android"
        platforms { "armv7a", "aarch64" }


    filter {"options:platform=win", "platforms:Win32"}
        architecture "x86"

    filter {"options:platform=win", "platforms:Win64"}
        architecture "x86_64"

    filter {"options:platform=macos"}
        architecture "arm64"

    filter {"options:platform=android", "platforms:armv7a"}
        architecture "arm"

    filter {"options:platform=android", "platforms:aarch64"}
        architecture "arm64"
    
    filter {}

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

    
    -- Android Arm64 (aarch64)
    --filter {"options:android64", "system:android"}
    --    architecture "ARM64"
--
    ---- Android Arm
    --filter {"options:android64", "system:android"}
    --    architecture "ARM"
--
    ---- Android x86
    --filter {"options:x86_build", "options: not arm_build", "system:android"}
    --    architecture "x86"
--
    ---- Android x64
    --filter {"options: not x86_build", "options: not arm_build", "system:android"}
    --    print("Build for Android x86_64")
    --    architecture "x86_64"
--
    ---- Windows x86
    --filter {"options:x86_build", "options: not arm_build", "system:windows"}
    --    print("Build for Windows x32")
    --    architecture "x86"
--
    ---- Windows x64
    --filter {"options: not x86_build", "options: not arm_build", "system:windows"}
    --    print("Build for Windows x64")
    --    architecture "x86_64"
--
    ---- MacOSX
    --filter {"system:macosx"}
    --    print("Build for MacOS")
    --    architecture "ARM64"