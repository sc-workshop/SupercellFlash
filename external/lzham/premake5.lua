
project "LZHAM"
    kind "StaticLib"

    language "C++"
	cppdialect "C++14"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "include",
        "src/lzhamcomp",
        "src/lzhamdecomp"
	}

    filter "configurations:Debug"
        runtime "Debug"

		defines {
            "_DEBUG",
            "_LIB"
        }

        symbols "on"
    
    filter "configurations:Release"
        runtime "Release"

        defines {
            "NDEBUG",
            "_LIB"
        }

        optimize "on"
		
	filter "architecture:x64"
		defines "LZHAM_64BIT"
		
	filter "system:windows"
		defines "WIN32"
