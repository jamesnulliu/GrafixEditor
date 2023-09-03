VULKAN = os.getenv("VULKAN_SDK")

workspace "GraphicsEditor"
    architecture "x64"
    startproject "DraftingBoard"

    configurations
    {
        "Debug",
        "Release"
    }

IncludeDir = {}
IncludeDir["ImGui"] = "GraphicsEditor/vendor/imgui"
IncludeDir["GLFW"] = "GraphicsEditor/vendor/GLFW/include"
IncludeDir["Vulkan"] = "%{VULKAN}/Include"

Library = {}
Library["Vulkan"] = "%{VULKAN}/Lib/vulkan-1.lib"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
    include "GraphicsEditor/vendor/imgui"
    include "GraphicsEditor/vendor/GLFW"
group ""


project "GraphicsEditor"
    location "GraphicsEditor"
    kind "StaticLib"
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
            "GE_WINDOWS"
        }

    filter "configurations:Debug"
        defines "GE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GE_RELEASE"
        runtime "Release"
        optimize "on"


project "DraftingBoard"
    location "DraftingBoard"
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
        "GraphicsEditor/src",
        "GraphicsEditor/vendor",
        "GraphicsEditor/vendor/spdlog/include",
    }

    links { "GraphicsEditor" }

    filter "system:windows"
        systemversion "latest"

        defines { "GE_WINDOWS" }

    filter "configurations:Debug"
        defines "GE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GE_RELEASE"
        runtime "Release"
        optimize "on"