workspace "Interpreter"
   configurations { "Debug", "Release" }
   platforms { "x64" }
   location "interpreter"  -- Specify where to place generated files
   startproject "vfScript"  -- Set the default startup project


-- Project 1: Editor
project "vfScript"
   kind "ConsoleApp"
   language "C"
   location "Interpreter/script"
   targetdir "bin/%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}"

   files { "Interpreter/script/**.h", "Interpreter/script/**.c" }

   defines { "_CRT_SECURE_NO_WARNINGS" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
