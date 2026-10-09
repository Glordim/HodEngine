project(TestsRenderer)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE Includes
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

assign_source_group(${Srcs}) # for VS

add_executable(TestsRenderer ${Srcs} ${Includes})
set_property(TARGET TestsRenderer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(TestsRenderer Renderer)

target_include_directories(TestsRenderer
	PRIVATE
		"../.."
)

# No gtest_main: main.cpp registers the environment that owns the Renderer
target_link_libraries(TestsRenderer
	PRIVATE
		Renderer
		GTest::gtest
)

target_compile_definitions(TestsRenderer PUBLIC ${HOD_PLATFORM_DEFINE})

# These tests need a Vulkan/Metal capable device, so they are discovered at test time
# rather than by running the executable during the build
include(GoogleTest)
gtest_discover_tests(TestsRenderer DISCOVERY_MODE PRE_TEST)
