workspace "SparseVoxelOctreeRenderer"
    architecture "x64"

    configurations { "Debug", "Release" } 

project "SparseVoxelOctreeRenderer"
    location "SparseVoxelOctreeRenderer"
    kind "WindowedApp"
    language "C++"
    targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}/") 
    objdir ("Intermediate/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}/")
    files{"%{prj.name}/**.h","%{prj.name}/**.cpp"}
    includedirs{"include","DXFramework/" }
    libdirs {"lib/%{cfg.buildcfg}/"}
    links {"DXFramework","d3d11.lib","DXFramework.lib","dxgi.lib", "D3DCompiler.lib"}

    filter "system:windows"
        cppdialect "C++17"
        systemversion "10.0.22621.0"


    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines 
        {
            "DEBUG"
        }
    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines 
        {
            "NDEBUG"
        }

project "DXFramework"
    location "DXFramework"
    kind "StaticLib"
    language "C++"
    targetdir ("lib/%{cfg.buildcfg}/") 
    objdir ("Intermediate/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}/")
    files{"%{prj.name}/**.h","%{prj.name}/**.cpp", "include/imGUI/**.cpp","include/imGUI/**.cpp"}
    includedirs{"include"}
    libdirs {"lib/%{cfg.buildcfg}/"}
    links {"DirectXTK.lib","assimp-vc141-mtd.lib"}

    filter "system:windows"
        cppdialect "C++17"
        systemversion "10.0.22621.0"


    filter "configurations:Debug"
        symbols "On"
        defines 
        {
            "DEBUG"
        }
    filter "configurations:Release"
        optimize "On"
        defines 
        {
            "NDEBUG"
        }