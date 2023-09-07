VULKAN = os.getenv("VULKAN_SDK")

workspace "Grafix"
    architecture "x64"
    startproject "GrafixEditor"

    configurations
    {
        "Debug",
        "Release"
    }

IncludeDir = {}
IncludeDir["ImGui"] = "Grafix/vendor/imgui"
IncludeDir["GLFW"] = "Grafix/vendor/glfw/include"
IncludeDir["Vulkan"] = "%{VULKAN}/Include"

Library = {}
Library["Vulkan"] = "%{VULKAN}/Lib/vulkan-1.lib"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
    include "Grafix/vendor/imgui"
    include "Grafix/vendor/glfw"
group ""


project "Grafix"
    location "Grafix"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "Grafix/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Vulkan}"
    }

    links
    {
        "ImGui",
        "GLFW",
        "%{Library.Vulkan}"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GF_WINDOWS"
        }

    filter "configurations:Debug"
        defines "GF_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GF_RELEASE"
        runtime "Release"
        optimize "on"


project "GrafixEditor"
    location "GrafixEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Grafix/src",
        "Grafix/vendor",
        "Grafix/vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Vulkan}"
    }

    links { "Grafix" }

    filter "system:windows"
        systemversion "latest"

        defines { "GF_WINDOWS" }

    filter "configurations:Debug"
        defines "GF_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GF_RELEASE"
        runtime "Release"
        optimize "on"