
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

workspace "SupercellFlash"
    configurations {
        "Debug",
        "Release"
    }

    filter {"options:platform=win"}
        system "windows"
        platforms { "Win64" }

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
	
	targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
	
	symbols "on"
	filter "configurations:Debug"
        runtime "Debug"

        defines {
            "DEBUG"
        }
        optimize "off"
    
    filter "configurations:Release"
        runtime "Release"

        defines {
            "NDEBUG"
        }
        optimize "Speed"
	
	filter ""

    include "./"
	include "core"
	include "Test"

    group "Compression"
		include "compression"
        include "compression/dependencies/lzma"
        include "compression/dependencies/lzham"
        include "compression/dependencies/zstd"
        include "compression/dependencies/astc"
