
cc_library(
	name = "box2d",
	srcs = glob(["src/**/*.cpp", "src/**/*.h", "include/**/*.h"]),
	hdrs = glob(["include/**/*.h"]),
	includes = [ "box2d" ],
	visibility = ["//visibility:public"],
	strip_include_prefix = "include",
	copts = [
		"-Iexternal/box2d/include",
		"-Iexternal/box2d",
		"-Iexternal/box2d/src",
	]
)