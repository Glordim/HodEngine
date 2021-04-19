# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

project(Application)
if (NOT TARGET Core)
include(../Core/CMakeLists.txt)
endif() 

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)

assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
".."
"${SDK_DIR}/SDL/include"
"${SDK_DIR}/glm"
)

add_library(Application ${Srcs})

add_dependencies(Application Core)

target_link_libraries(Application
Core
optimized "${SDK_DIR}/SDL/Release/SDL2.lib" debug "${SDK_DIR}/Build/SDL/Debug/SDL2d.lib"
optimized "${SDK_DIR}/SDL/Release/SDL2main.lib" debug "${SDK_DIR}/Build/SDL/Debug/SDL2maind.lib"
)
