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

project(Game)
if (NOT TARGET Core)
include(../Core/CMakeLists.txt)
endif()
if (NOT TARGET Renderer)
include(../Renderer/CMakeLists.txt)
endif() 

file(GLOB_RECURSE Srcs
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
		"${PROJECT_SOURCE_DIR}/src/*.inl"
        )

assign_source_group(${Srcs})
		
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
".."
"../../SDK/SDL/include"
"../../SDK/glm"
"../../SDK/tinyobjloader"
"../../SDK/stb"
"../../SDK/SPIRV-Cross"
"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/include"
)
		
add_library(Game ${Srcs})

target_link_libraries(Game
optimized "../../../Build/HodEngine/Core/Release/Core" debug "../../../Build/HodEngine/Core/Debug/Core"
optimized "../../../Build/HodEngine/Physic/Release/Physic" debug "../../../Build/HodEngine/Physic/Debug/Physic"
optimized "../../../Build/HodEngine/ImGui/Release/ImGui" debug "../../../Build/HodEngine/ImGui/Debug/ImGui"
optimized "../../../Build/HodEngine/Application/Release/Application" debug "../../../Build/HodEngine/Application/Debug/Application"
optimized "../../../Build/HodEngine/Renderer/Release/Renderer" debug "../../../Build/HodEngine/Renderer/Debug/Renderer"
)