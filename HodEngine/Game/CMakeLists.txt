cmake_minimum_required(VERSION 3.4)

project(Game)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../CMake)
find_package(SDL2 REQUIRED)
find_package(RapidJSON REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
	".."
	${GLM_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
	${RapidJSON_INCLUDE_DIRS}
)

add_library(Game ${Srcs})

add_dependencies(Game Core DebugLayer ImGui Application Physics Renderer)

target_link_libraries(Game
	Core
	DebugLayer
	ImGui
	Application
	Physics
	Renderer
	${SDL2_LIBRARIES}
)
