

project "SupercellFlash"
    kind "StaticLib"

    language "C++"
    cppdialect "C++17"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    
    files {
        "src/**.h",
        "src/**.cpp",
        
        "external/**.h",
        "external/**.c",
        "external/**.cpp"
    }

    includedirs {
        "src",

        -- md5
        "external/md5",

        -- lzma
        "external/lzma/include",

        -- lzham
        "external/lzham/include",
        "external/lzham/src/lzhamcomp",
        "external/lzham/src/lzhamdecomp",

        -- zstandard
        "external/zstd/include",
        "external/zstd/include/common",
        "external/zstd/include/compress",
        "external/zstd/include/decompress"
    }

    filter "configurations:Debug"
        runtime "Debug"
        defines { "SC_DEBUG" }

        -- for lzham
        defines {
            "_DEBUG",
            "_LIB"
        }

        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        defines { "SC_RELEASE" }

        -- for lzham
        defines {
            "NDEBUG",
            "_LIB"
        }

        optimize "on"

    -- for lzham
    filter "architecture:x64"
        defines "LZHAM_64BIT"

    filter "system:windows"
        defines "WIN32"

