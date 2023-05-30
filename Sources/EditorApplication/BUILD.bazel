load("@rules_cc//cc:defs.bzl", "cc_binary")

platform_define = "PLATFORM_WINDOWS"

cc_binary(
	name = "HodEditor",
	srcs = glob(["**/*.cpp", "**/*.h", "**/*.inl"]),
	deps = ["//Sources/HodEngine:HodEngine"],
	defines = [platform_define],
	copts = [
		#"-std=c++17",
		"/std:c++20",
	],
)
