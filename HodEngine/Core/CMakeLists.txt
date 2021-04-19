# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

project(Core)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)

assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
".."
"${SDK_DIR}/glm"
)

add_library(Core ${Srcs})

target_link_libraries(Core
optimized "${SDK_DIR}/Build/glm/glm/Release/glm_static.lib" debug "${SDK_DIR}/Build/glm/glm/Debug/glm_static.lib"
)
