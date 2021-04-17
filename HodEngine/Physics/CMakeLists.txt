# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source_rel}")
    endforeach()
endfunction(assign_source_group)

project(Physics)
if (NOT TARGET Core)
include(../Core/CMakeLists.txt)
endif() 
if (NOT TARGET DebugLayer)
include(../DebugLayer/CMakeLists.txt)
endif() 

file(GLOB_RECURSE Srcs
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
		"${PROJECT_SOURCE_DIR}/src/*.inl"
        )

assign_source_group(${Srcs})
		
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
".."
"../../SDK/glm"
"../../SDK/box2d/include"
)

add_library(Physics ${Srcs})

add_dependencies(Physics Core DebugLayer)

target_link_libraries(Physics
optimized "../../../Build/HodEngine/Core/Release/Core" debug "../../../Build/HodEngine/Core/Debug/Core"
optimized "../../../Build/HodEngine/DebugLayer/Release/DebugLayer" debug "../../../Build/HodEngine/DebugLayer/Debug/DebugLayer"
optimized "../../../SDK/Build/box2d/bin/Release/box2d" debug "../../../SDK/Build/box2d/bin/Debug/box2d"
)