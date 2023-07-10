load("@rules_cc//cc:defs.bzl", "cc_library")

platform_define = "PLATFORM_WINDOWS"
platform_lib = "-DEFAULTLIB:Kernel32.lib -DEFAULTLIB:Advapi32.lib -DEFAULTLIB:Comdlg32.lib"

cc_library(
	name = "HodEngine",
	srcs = glob([
		"Core/**/*.cpp",
		"Application/**/*.cpp",
		"Editor/**/*.cpp",
		"Physics/**/*.cpp",
		"Game/**/*.cpp",
		"ImGui/**/*.cpp",
		"Renderer/**/*.cpp",
		"Window/**/*.cpp"
	]),
	hdrs = glob([
		"Core/**/*.h",
		"Core/**/*.inl",
		"Application/**/*.h",
		"Application/**/*.inl",
		"Editor/**/*.h",
		"Editor/**/*.inl",
		"Physics/**/*.h",
		"Physics/**/*.inl",
		"Game/**/*.h",
		"Game/**/*.inl",
		"ImGui/**/*.h",
		"ImGui/**/*.inl",
		"Renderer/**/*.h",
		"Renderer/**/*.inl",
		"Window/**/*.h",
		"Window/**/*.inl"
	]),
	deps = [
		"@glm//:glm",
		"@stb//:stb",
		"@box2d//:box2d",
		"@rules_vulkan//vulkan:vulkan_cc_library"
	],
	defines = [platform_define, "RENDERER_ENABLE_VALIDATION_LAYER"],
	linkopts = [platform_lib],
	copts = [
		#"-std=c++17",
		"/std:c++20",
	],
	visibility = ["//visibility:public"],
	include_prefix = "HodEngine",
)
