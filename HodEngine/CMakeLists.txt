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

# Notre projet est étiqueté hello
project(HodEngine)
	
file(GLOB_RECURSE Srcs
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
		"${PROJECT_SOURCE_DIR}/src/*.inl"
        )
	
assign_source_group(${Srcs})

set(IncludeDirs "${PROJECT_SOURCE_DIR}/src" "../glm" "../PhysX/physx/include" "../PhysX/pxshared/include" "../SDL/include" "../tinyobjloader" "../stb" "../SPIRV-Cross" "C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/include")
set(Libs
"../PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysX_64"
"../PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXCommon_64"
"../PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXExtensions_static_64"
"../PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXFoundation_64"
"../SDL/VisualC/x64/Release/SDL2"
"../SPIRV-Cross/Build/Release/spirv-cross-core"
"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/release/libfbxsdk.lib"
"d3d12.lib"
"DXGI.lib")

include_directories(${IncludeDirs})

add_executable(HodEngine WIN32 ${Srcs})

target_link_libraries(HodEngine ${Libs})

find_package(Vulkan)
if (Vulkan_FOUND)
  # Une fois la dépendance trouvée, nous l'incluons au projet
  target_include_directories(HodEngine PUBLIC ${Vulkan_INCLUDE_DIRS})
  target_link_libraries (HodEngine ${Vulkan_LIBRARIES})
else ()
  # Sinon, nous affichons un message
  message(FATAL_ERROR "Vulkan not found")
endif ()
