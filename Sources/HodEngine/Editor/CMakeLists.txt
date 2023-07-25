cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Editor)

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

find_package(Stb REQUIRED)

add_library(Editor OBJECT ${Srcs} ${Includes})

add_dependencies(Editor Core ImGui Window Physics Renderer)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${Core_INCLUDES}
	${Stb_INCLUDE_DIRS}
)

target_link_libraries(Editor
	Core
	ImGui
	Window
	Physics
	Renderer
)
