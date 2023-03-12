
project "SupercellCompression"
    kind "StaticLib"

    language "C++"
    cppdialect "C++17"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
	
	-- Sources
	files {
		"src/**.cpp"
	}
	
	-- Headers
	files {
		"include/**.h",
		"src/**.h"
	}

    includedirs {
        "include",
		"src",
		"%{wks.location}/SupercellBytestream/include",
		"%{wks.location}/external/lzham/include",
		"%{wks.location}/external/lzma/include",
		"%{wks.location}/external/zstd/include"
	}
	
	links {
        "LZMA",
		"LZHAM",
		"Zstandard"
    } 

    filter "configurations:Debug"
        runtime "Debug"

        defines {
            "SC_DEBUG"
        }

        symbols "on"
    
    filter "configurations:Release"
        runtime "Release"

        defines {
            "SC_RELEASE"
        }

        optimize "on"