cmake_minimum_required(VERSION 3.4)

project(Application)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../CMake)
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
	".."
	${SDL2_INCLUDE_DIRS}
)

add_library(Application ${Srcs})

add_dependencies(Application Core)

target_link_libraries(Application
	Core
	${SDL2_LIBRARIES}
	${SDL2MAIN_LIBRARY}
)
