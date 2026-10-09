include(EmbedShader)

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

# Shaders only the tests use, compiled and embedded the way the Renderer does for its own
get_target_property(API RHI HOD_RHI_BACKEND)

file(GLOB_RECURSE ShaderSources "${PROJECT_SOURCE_DIR}/Shader/*.slang")

set(GENERATED_DIR ${PROJECT_BINARY_DIR}/Generated)
set(SHADER_TEMPLATE_DIR ${CMAKE_SOURCE_DIR}/Sources/HodEngine/Renderer/EmbedInSourceTemplate)

set(GENERATED_SHADER_CPPS "")
set(GENERATED_SHADER_HPPS "")
EmbedShaders(ShaderSources ${API} Debug ${SHADER_TEMPLATE_DIR}/ShaderSourceFileTemplate.cpp.in ${SHADER_TEMPLATE_DIR}/ShaderSourceFileTemplate.hpp.in ${GENERATED_DIR}/Shader ${PROJECT_BINARY_DIR}/ShaderTmp GENERATED_SHADER_CPPS GENERATED_SHADER_HPPS)
list(APPEND Srcs ${GENERATED_SHADER_CPPS})
list(APPEND Includes ${GENERATED_SHADER_HPPS})

assign_source_group(${Srcs}) # for VS

add_executable(TestsRenderer ${Srcs} ${Includes})
set_property(TARGET TestsRenderer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(TestsRenderer Renderer)

target_include_directories(TestsRenderer
	PRIVATE
		"../.."
		${GENERATED_DIR}
)

# No gtest_main: main.cpp registers the environment that owns the Renderer
target_link_libraries(TestsRenderer
	PRIVATE
		Renderer
		GTest::gtest
)

target_compile_definitions(TestsRenderer
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE TESTS_RENDERER_FIXTURES_DIR="${PROJECT_SOURCE_DIR}/Fixtures"
)

# These tests need a Vulkan/Metal capable device, so they are discovered at test time
# rather than by running the executable during the build
include(GoogleTest)
gtest_discover_tests(TestsRenderer DISCOVERY_MODE PRE_TEST)
