-- premake5.lua
workspace "GemCompiler"
   location ("../../")
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "GemCompiler"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "../../GemCompiler/Build-Compiler.lua"

