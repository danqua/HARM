workspace "HARM"
    architecture "x86_64"
    startproject "HARM"
    
    location ".."

    configurations {
        "Debug",
        "Release"
    }


    flags {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    targetdir ("../Build/" .. outputdir .. "/%{prj.name}")
    objdir ("../Build/Intermediate/" .. outputdir .. "/%{prj.name}")
    
    debugdir ("../Content")

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "Speed"
        symbols "On"
        runtime "Release"

project "Engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"
    
    location "../Intermediate/ProjectFiles"


    files {
        "../Source/Engine/**.h",
        "../Source/Engine/**.cpp",
        "../ThirdParty/glad-4.3/src/glad.c"
    }

    includedirs {
        "../Source",
        "../ThirdParty/glad-4.3/include",
    }

    filter "toolset:msc*"
        rtti "Off"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter { "toolset:msc", "configurations:Release" }
        buildoptions "/GT"
        linkoptions "/LTCG"

project "Game"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"
    
    location "../Intermediate/ProjectFiles"

    files {
        "../Source/Game/**.h",
        "../Source/Game/**.cpp"
    }

    includedirs {
        "../Source",
    }

    links {
        "Engine"
    }

    defines {
        "GAME_EXPORTS"
    }

    filter "toolset:msc*"
        rtti "Off"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "system:windows"
        postbuildcommands {
            "copy \"$(TargetPath)\" \"$(SolutionDir)Content\\\""
        }

project "HARM"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    
    files {
        "../Source/Main.cpp",
    }
    
    includedirs {
        "../Source/",
        "../ThirdParty/SDL3-3.2.8/include",
    }
    
    libdirs {
        "../ThirdParty/SDL3-3.2.8/lib/x64",
    }
    
    links {
        "Engine",
        "SDL3"
    }
    
    filter "system:windows"
        postbuildcommands {
            "copy \"$(SolutionDir)ThirdParty\\SDL3-3.2.8\\lib\\x64\\SDL3.dll\" \"$(SolutionDir)Content\\\""
        }
    
    dependson { "Game" }