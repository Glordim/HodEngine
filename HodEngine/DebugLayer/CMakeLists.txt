cmake_minimum_required(VERSION 3.4)

project(DebugLayer)

file(GLOB_RECURSE Srcs
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

add_library(DebugLayer ${Srcs})

target_link_libraries(
	Core
)

add_dependencies(DebugLayer Core)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
    ".."
	${Core_INCLUDES}
)
