
project "SupercellFlash"
    kind "StaticLib"

    language "C++"
    cppdialect "C++17"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"

    files {
		"include/**.h",
		"src/**.cpp",
		"src/**.h"
    }

    includedirs {
        "src",
		"include",
		"dependencies/Bytestream",
        "dependencies/Compression/include",
        "dependencies/TextureLoader/include"
    }

    links {
        "SupercellCompression",
        "SupercellTextureLoader",
		"LZMA",
		"LZHAM",
		"Zstandard"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "on"

