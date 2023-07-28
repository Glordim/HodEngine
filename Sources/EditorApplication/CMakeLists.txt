cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(EditorApplication)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.inl"
)

file(GLOB_RECURSE Includes
"${PROJECT_SOURCE_DIR}/*.h"
"${PROJECT_SOURCE_DIR}/*.inl"
)

assign_source_group(${Srcs}) # for VS

add_executable(EditorApplication ${Srcs} ${Includes})
set_property(TARGET EditorApplication PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

include_directories(
	".."
	${Glm_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}
)

add_dependencies(EditorApplication Engine)

target_link_libraries(EditorApplication
	Engine
)

target_compile_definitions(EditorApplication PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
