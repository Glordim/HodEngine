cmake_minimum_required(VERSION 3.4)

project(DebugLayer)

file(GLOB_RECURSE Srcs
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
    ".."
)

add_library(DebugLayer ${Srcs})

add_dependencies(DebugLayer Core)

target_link_libraries(
	Core
)
