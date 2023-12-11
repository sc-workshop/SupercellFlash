
project "SupercellFlash"
    kind "StaticLib"

    language "C++"
    cppdialect "C++17"

    files {
		"include/**",
		"source/**"
    }
	
	includedirs
	{
		"./include",
		"./core",
		"./compression/include"
	}
	
    links {
		"Core",
        "Compression"
    }
	
	defines 
	{
		"_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING"
	}


