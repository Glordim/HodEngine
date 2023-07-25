cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Game)

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
)

add_library(Game OBJECT ${Srcs} ${Includes})

add_dependencies(Game Core ImGui Window Physics Renderer)

target_link_libraries(Game
	Core
	ImGui
	Window
	Physics
	Renderer
)
