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

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "Speed"
        symbols "On"

project "HARM"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    
    location "../Intermediate/ProjectFiles"

    targetdir ("../Build/" .. outputdir .. "/%{prj.name}")
    objdir ("../Build/Intermediate/" .. outputdir .. "/%{prj.name}")
    debugdir ("../Content")

    files {
        "../Source/**.h",
        "../Source/**.cpp",
        "../ThirdParty/glad-4.3/src/glad.c"
    }

    includedirs {
        "../Source",

        "../ThirdParty/SDL3-3.2.8/include",
        "../ThirdParty/glad-4.3/include",
    }

    libdirs {
        "../ThirdParty/SDL3-3.2.8/lib/x64"
    }

    links {
        "SDL3"
    }

    filter "toolset:msc*"
        rtti "Off"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter { "toolset:msc", "configurations:Release" }
        buildoptions "/GT"
        linkoptions "/LTCG"

    filter "system:windows"
        postbuildcommands {
            "copy \"$(SolutionDir)ThirdParty\\SDL3-3.2.8\\lib\\x64\\SDL3.dll\" \"$(SolutionDir)Content\\\""
        }
    