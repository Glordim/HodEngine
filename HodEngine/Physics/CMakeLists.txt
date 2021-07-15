cmake_minimum_required(VERSION 3.4)

project(Physics)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../CMake)
find_package(Box2d REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
	".."
	${Box2d_INCLUDE_DIRS}
)

add_library(Physics STATIC ${Srcs})

add_dependencies(Physics Core DebugLayer Renderer)

target_link_libraries(Physics
	Core
	DebugLayer
	Renderer
	${Box2d_LIBRARIES}
)
