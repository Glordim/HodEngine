# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

project(Physics)

file(GLOB_RECURSE Srcs
		"${PROJECT_SOURCE_DIR}/src/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/*.h"
		"${PROJECT_SOURCE_DIR}/src/*.inl"
)

assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
".."
"${SDK_DIR}/glm"
"${SDK_DIR}/box2d/include"
)

add_library(Physics ${Srcs})

add_dependencies(Physics Core DebugLayer Renderer)

target_link_libraries(Physics
Core
DebugLayer
Renderer
optimized "${SDK_DIR}/Build/box2d/bin/Release/box2d.lib" debug "${SDK_DIR}/Build/box2d/bin/Debug/box2d.lib"
)
