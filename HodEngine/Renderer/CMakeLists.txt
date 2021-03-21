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

project(Renderer)
if (NOT TARGET Core)
include(../Core/CMakeLists.txt)
endif() 
if (NOT TARGET DebugLayer)
include(../DebugLayer/CMakeLists.txt)
endif() 
if (NOT TARGET Application)
include(../Application/CMakeLists.txt)
endif() 

file(GLOB_RECURSE Srcs
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
		"${PROJECT_SOURCE_DIR}/src/*.inl"
        )

assign_source_group(${Srcs})
		
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
".."
"../../SDK"
"../../SDK/glm"
"../../SDK/tinyobjloader"
"../../SDK/stb"
"../../SDK/SPIRV-Cross"
"../../SDK/rapidjson/include"
"C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/include"
)

add_library(Renderer ${Srcs})

add_dependencies(Renderer Core DebugLayer Application)

target_link_libraries(Renderer
optimized "../../../Build/HodEngine/Core/Release/Core" debug "../../../Build/HodEngine/Core/Debug/Core"
optimized "../../../Build/HodEngine/DebugLayer/Release/DebugLayer" debug "../../../Build/HodEngine/DebugLayer/Debug/DebugLayer"
optimized "../../../Build/HodEngine/ImGui/Release/ImGui" debug "../../../Build/HodEngine/ImGui/Debug/ImGui"
optimized "../../../Build/HodEngine/Application/Release/Application" debug "../../../Build/HodEngine/Application/Debug/Application"
optimized "../../../SDK/Build/SPIRV-Cross/Release/spirv-cross-core" debug "../../../SDK/Build/SPIRV-Cross/Debug/spirv-cross-core"
optimized "../../../SDK/Build/tinyobjloader/Release/tinyobjloader" debug "../../../SDK/Build/tinyobjloader/Debug/tinyobjloader"
optimized "../../../SDK/Build/glm/glm/Release/glm_static" debug "../../../SDK/Build/glm/glm/Debug/glm_static"
optimized "C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/release/libfbxsdk.lib" debug "C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/debug/libfbxsdk.lib"
optimized "d3d12.lib" debug "d3d12.lib"
optimized "DXGI.lib" debug "DXGI.lib"
)

find_package(Vulkan)
if (Vulkan_FOUND)
  # Une fois la dépendance trouvée, nous l'incluons au projet
  target_include_directories(Renderer PUBLIC ${Vulkan_INCLUDE_DIRS})
  target_link_libraries (Renderer ${Vulkan_LIBRARIES})
else ()
  # Sinon, nous affichons un message
  message(FATAL_ERROR "Vulkan not found")
endif ()