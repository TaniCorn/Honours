workspace "SparseVoxelOctreeRenderer"
    architecture "x64"
    startproject "SparseVoxelOctreeRenderer"
    configurations { "Debug", "Release" } 
    local targetBuildLocation = "bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    -- Currently not working

    filter "system:windows"
    postbuildcommands {
        'xcopy /E /Y /I "%{wks.location}\\res" "%{cfg.targetdir}\\res\\"'
    }
    
    filter "system:linux or macosx"
    postbuildcommands {
        'cp -r "%{wks.location}/res" "%{cfg.targetdir}/res"'
    }

    filter {} -- clear filter

project "SparseVoxelOctreeRenderer"
    location "SparseVoxelOctreeRenderer"
    kind "WindowedApp"
    language "C++"
    targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}/") 
    objdir ("Intermediate/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}/")
    files{"%{prj.name}/**.h","%{prj.name}/**.cpp", "%{prj.name}/**.hlsl", "%{prj.name}/**.hlsli"}
    includedirs{"include","DXFramework/" ,
    "%{prj.name}/Include/Scene",
    "%{prj.name}/Include/Util",
    "%{prj.name}/Include/Shaders",
    "%{prj.name}/Include/Voxels",
    "%{prj.name}/Include/Octree",
    "include/Perfkit/NvPerfUtility/",
    "include/Perfkit/windows-desktop-x64",
    "include/Perfkit/",
    -- Update with any extra files in the include
    }
    libdirs {"lib/%{cfg.buildcfg}/", "ExternalLib/%{cfg.buildcfg}/"}
    links {"DXFramework","d3d11.lib","DXFramework.lib","dxgi.lib", "D3DCompiler.lib"}

    -- Sets the working directory to the output where the .cso files get outputted. 
    -- Otherwise set each hlsl file to output to the defualt working directory.
    debugdir "bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}/"

    -- Shader setup
    filter "files:**.hlsl"
        shadermodel "5.0"
        shaderentry "main"
    filter "files:**ps.hlsl"
        shadertype "Pixel"
    filter "files:**vs.hlsl"
        shadertype "Vertex"
    filter "files:**cs.hlsl"
        shadertype "Compute"

    
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
    libdirs {"lib/%{cfg.buildcfg}/", "ExternalLib/%{cfg.buildcfg}/"}
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


