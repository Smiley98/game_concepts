newoption
{
	trigger = "graphics",
	value = "OPENGL_VERSION",
	description = "version of OpenGL to build raylib against",
	allowed = {
		{ "opengl11", "OpenGL 1.1"},
		{ "opengl21", "OpenGL 2.1"},
		{ "opengl33", "OpenGL 3.3"},
		{ "opengl43", "OpenGL 4.3"}
	},
	default = "opengl33"
}

function define_C()
	language "C"
end

function define_Cpp()
	language "C++"
end

function string.starts(String,Start)
	return string.sub(String,1,string.len(Start))==Start
end

function link_to(lib)
	links (lib)
	includedirs ("../"..lib.."/include", "../"..lib.."/" )
end

function download_progress(total, current)
	local ratio = current / total;
	ratio = math.min(math.max(ratio, 0), 1);
	local percent = math.floor(ratio * 100);
	print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
	if(os.isdir("raylib") == false and os.isdir("raylib-master") == false) then
		if(not os.isfile("raylib-master.zip")) then
			print("Raylib not found, downloading from github")
			local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib-master.zip", {
				progress = download_progress,
				headers = { "From: Premake", "Referer: Premake" }
			})
		end
		print("Unzipping to " ..  os.getcwd())
		zip.extract("raylib-master.zip", os.getcwd())
		os.remove("raylib-master.zip")
	end
end

function check_imgui()
	if(os.isdir("imgui") == false and os.isdir("imgui-master") == false) then
		if(not os.isfile("imgui-master.zip")) then
			print("imgui not found, downloading from github")
			local result_str, response_code = http.download("https://github.com/ocornut/imgui/archive/refs/heads/master.zip", "imgui-master.zip", {
				progress = download_progress,
				headers = { "From: Premake", "Referer: Premake" }
			})
		end
		print("Unzipping to " ..  os.getcwd())
		zip.extract("imgui-master.zip", os.getcwd())
		os.remove("imgui-master.zip")
	end
end

workspace "game_concepts"
	configurations { "Debug", "Release" }
	platforms { "x64"}
	defaultplatform "x64"
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"	
		
	filter { "platforms:x64" }
		architecture "x86_64"
		
	targetdir "bin/%{cfg.buildcfg}/"
	

	cdialect "C99"
	cppdialect "C++11"
	check_raylib()
	check_imgui()

	include ("raylib_premake5.lua")
		
project "rlImGui"
	kind "StaticLib"
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	language "C++"
	
	include_raylib()
	includedirs { "rlImGui", "imgui", "imgui-master"}
	vpaths 
	{
		["Header Files"] = { "*.h"},
		["Source Files"] = {"*.cpp"},
		["ImGui Files"] = { "imgui/*.h","imgui/*.cpp", "imgui-master/*.h","imgui-master/*.cpp" },
	}
	files {"imgui-master/*.h", "imgui-master/*.cpp", "imgui/*.h", "imgui/*.cpp", "*.cpp", "*.h", "extras/**.h"}

	defines {"IMGUI_DISABLE_OBSOLETE_FUNCTIONS","IMGUI_DISABLE_OBSOLETE_KEYIO"}

--[[
group "Examples"
project "simple"
	kind "ConsoleApp"
	language "C++"
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	
	vpaths 
	{
		["Header Files"] = { "examples/**.h"},
		["Source Files"] = {"examples/**.cpp", "examples/**.c"},
	}
	files {"examples/simple.cpp"}
	link_raylib()
	links {"rlImGui"}
	includedirs {"./", "imgui", "imgui-master" }
		
project "editor"
	kind "ConsoleApp"
	language "C++"
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	
	vpaths 
	{
		["Header Files"] = { "examples/**.h"},
		["Source Files"] = {"examples/**.cpp", "examples/**.c"},
	}
	files {"examples/editor.cpp"}
	link_raylib()
	links {"rlImGui"}
	includedirs {"./", "imgui", "imgui-master" }
--]]

project "game"
	kind "ConsoleApp"
	language "C++"
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	
	vpaths 
	{
		["Header Files"] = {"game/src/**.h"},
		["Source Files"] = {"game/src/**.cpp"},
	}
	files {"game/src/**.h", "game/src/**.cpp"}
	link_raylib()
	links {"rlImGui"}
	includedirs {"./", "imgui", "imgui-master" }
