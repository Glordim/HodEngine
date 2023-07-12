load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
	name = "glm",
	url = "https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.zip",
	sha256 = "4605259c22feadf35388c027f07b345ad3aa3b12631a5a316347f7566c6f1839",
	build_file = "@//bazel:glm.BUILD",
	strip_prefix = "glm-0.9.9.8",
)

http_archive(
	name = "stb",
	url = "https://github.com/nothings/stb/archive/refs/heads/master.zip",
	build_file = "@//bazel:stb.BUILD",
	strip_prefix = "stb-master",
)

http_archive(
	name = "box2d",
	url = "https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.zip",
	sha256 = "0cb512dfa5be79ca227cd881b279adee61249c85c8b51caf5aa036b71e943002",
	build_file = "@//bazel:box2d.BUILD",
	strip_prefix = "box2d-2.4.1",
)

#http_archive(
#	name = "rules_vulkan",
#	url = "https://github.com/jadarve/rules_vulkan/archive/refs/tags/v0.0.9.zip",
#	sha256 = "9ea074c1de07bd41b0705dcb96e7cc50bf3a087fc5604ce2befd24ac70d6da32",
#	strip_prefix = "rules_vulkan-0.0.9",
#	#build_file = "BUILD",
#	#strip_prefix = "box2d-2.4.1",
#)

local_repository(
	name = "rules_vulkan",
	path = "rules_vulkan",
	#build_file = "external/rules_vulkan.BUILD",
)

load("@rules_vulkan//vulkan:repositories.bzl", "vulkan_repositories")
vulkan_repositories(
	sdk_path = "",
	android_use_host_vulkan_sdk = False
)

http_archive(
	name = "hedron_compile_commands",

	# Replace the commit hash in both places (below) with the latest, rather than using the stale one here.
	# Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
	url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/refs/heads/main.zip",
	strip_prefix = "bazel-compile-commands-extractor-main",
	# When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'hedron_compile_commands' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
)
#load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")
#hedron_compile_commands_setup()
