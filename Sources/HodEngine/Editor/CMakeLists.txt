cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Editor)

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

find_package(Stb REQUIRED)
find_package(PortableFileDialog REQUIRED)

add_library(Editor OBJECT ${Srcs} ${Includes})
set_property(TARGET Editor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Editor Core ImGui Window Physics Renderer)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${Core_INCLUDES}
	${Stb_INCLUDE_DIRS}
	${PortableFileDialog_INCLUDE_DIRS}
)

target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")

target_link_libraries(Editor
	Core
	ImGui
	Window
	Physics
	Renderer
)
