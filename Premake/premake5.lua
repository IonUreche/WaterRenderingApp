
PROJECT_NAME = "Water Rendering App"
ROOT = "D:/Projects/CUDA/WaterRenderingApp"
local prj_base = ROOT

workspace "HelloWorld"
    configurations { "Debug", "Release" }
  
project (PROJECT_NAME)
    language "C++"
    kind "ConsoleApp"
    targetdir "bin/%{cfg.buildcfg}"

    files { prj_base .. '/**.cpp',
            prj_base .. '/**.h',
            prj_base .. '/**.ui',
            prj_base .. '/**.qrc',
            prj_base .. '/**.rc'}

    filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

    filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"