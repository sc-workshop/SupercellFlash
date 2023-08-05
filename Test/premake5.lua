project "SupercellFlashTest"
    kind "ConsoleApp"

    language "C++"
    cppdialect "C++17"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"

    files {
		"Main.cpp"
    }

    includedirs {
        "%{wks.location}/include",
		"%{wks.location}/dependencies/Bytestream",
        "%{wks.location}/dependencies/Compression/include"
    }

    links {
        "SupercellCompression",
		"LZMA",
		"LZHAM",
		"Zstandard",
		"ASTC",
		
		"SupercellFlash",
		"SupercellTextureLoader",
		
		"libktx",
		"dfdutils",
		"basisu",
		"ETCPACK"
		
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "on"