# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

project(DebugLayer)
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
"${SDK_DIR}/glm"
)

add_library(DebugLayer ${Srcs})

add_dependencies(DebugLayer Core)
