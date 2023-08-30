workspace "GraphicsEditor"
    architecture "x64"
    startproject "GraphicsEditor"

    configurations
    {
        "Debug",
        "Release"
    }

IncludeDir = {}
IncludeDir["ImGui"] = "GraphicsEditor/vendor/imgui"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
    include "GraphicsEditor/vendor/imgui"
group ""


project "GraphicsEditor"
    location "GraphicsEditor"
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
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.ImGui}"
    }

    links
    {
        "ImGui",
        "d3d9.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GRAPHICS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "GRAPHICS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GRAPHICS_RELEASE"
        runtime "Release"
        optimize "on"


