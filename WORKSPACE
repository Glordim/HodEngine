load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
	name = "glm",
	url = "https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.zip",
	sha256 = "4605259c22feadf35388c027f07b345ad3aa3b12631a5a316347f7566c6f1839",
	build_file = "glm.BUILD",
	strip_prefix = "glm-0.9.9.8",
)

http_archive(
	name = "stb",
	url = "https://github.com/nothings/stb/archive/refs/heads/master.zip",
	sha256 = "10ab9ed5defe6e4adbc16d2aab58bbddaa4bd754c23eff37fe593e2ef8ebe51a",
	build_file = "stb.BUILD",
	strip_prefix = "stb-master",
)

http_archive(
	name = "box2d",
	url = "https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.zip",
	sha256 = "0cb512dfa5be79ca227cd881b279adee61249c85c8b51caf5aa036b71e943002",
	build_file = "box2d.BUILD",
	strip_prefix = "box2d-2.4.1",
)

# TODO omg Windows only
new_local_repository(
    name = "vulkan",
    path = "C:\\VulkanSDK\\1.3.243.0\\",
    build_file = "external/vulkan.BUILD",
)
