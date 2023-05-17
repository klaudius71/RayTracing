workspace "RayTracing"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "RayTracing"
    flags { "MultiProcessorCompile" }
    warnings "Default"

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "Iceberg/Iceberg.lua"

project "RayTracing"
    location "RayTracing"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    debugdir "%{wks.location}"

    files
    {
        "RayTracing/src/**.h",
        "RayTracing/src/**.cpp"
    }

    links 
    { 
        "Iceberg"
    }

    includedirs 
    { 
        "Iceberg/src",
        "Iceberg/src/Core",
        "Iceberg/src/GraphicsContext",
        "Iceberg/src/GraphicsContext/Buffers",
        "Iceberg/src/GraphicsContext/Pipeline",
        "Iceberg/src/GraphicsContext/Texture",
        "Iceberg/external/glfw/include",
        "Iceberg/external/glm",
        "Iceberg/external/imgui",
        "Iceberg/external/stb"
    }

    filter { "options:VulkanSDK=volk" }
        defines { "ICEBERG_VOLK" }
        includedirs 
        { 
            "Iceberg/external/volk",
            "Iceberg/external/Vulkan-Headers/include" 
        }

    filter { "platforms:x64" }
        architecture "x64"
    
    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    
    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"
