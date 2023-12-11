project "SupercellFlashTest"
    kind "ConsoleApp"

    language "C++"
    cppdialect "C++17"
	
    files {
		"Main.cpp"
    }

    includedirs {
        "%{wks.location}/include/",
		"%{wks.location}/core/",
        "%{wks.location}/compression/include/"
    }

    links {
		"SupercellFlash"
    }