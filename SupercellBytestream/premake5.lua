
project "SupercellBytestream"
    kind "StaticLib"

    language "C++"
    cppdialect "C++17"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"

	files {
		"include/**.h"
	}

    includedirs {
        "include"
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