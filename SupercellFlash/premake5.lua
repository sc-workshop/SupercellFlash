
project "SupercellFlash"
    kind "StaticLib"

    language "C++"
    cppdialect "C++17"

    targetdir "%{OutputLibsDir}/%{prj.name}"
    objdir "%{InterLibsDir}/%{prj.name}"

    files {
		"include/**.h",
		"src/**.cpp",
		"src/**.h"
    }

    includedirs {
        "src",
		"include",
		"%{wks.location}/SupercellBytestream/include",
        "%{wks.location}/SupercellCompression/include"
    }

    links {
        "SupercellCompression",
		"LZMA",
		"LZHAM",
		"Zstandard"
    }
	
	defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        defines { "SC_DEBUG" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "SC_RELEASE" }
        runtime "Release"
        optimize "on"

