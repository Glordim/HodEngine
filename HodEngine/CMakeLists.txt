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

add_executable(HodEngine ${Srcs})

set_property(TARGET HodEngine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "../Data")

set(IncludeDirs "${PROJECT_SOURCE_DIR}/src" "../SDK/glm" "../SDK/PhysX/physx/include" "../SDK/PhysX/pxshared/include" "../SDK/SDL/include" "../SDK/tinyobjloader" "../SDK/stb" "../SDK/SPIRV-Cross" "C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/include")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

 set(Libs
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysX_64"
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXCommon_64"
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXExtensions_static_64"
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXFoundation_64"
"../SDK/SDL/VisualC/x64/Release/SDL2"
"../SDK/SDL/VisualC/x64/Release/SDL2main"
"../SDK/SPIRV-Cross/Build/Release/spirv-cross-core"
"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/release/libfbxsdk.lib"
"d3d12.lib"
"DXGI.lib")

add_custom_command(TARGET HodEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "../SDK/SDL/VisualC/x64/Release/SDL2.dll"
		"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysX_64.dll"
		"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXCommon_64.dll"
		"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXFoundation_64.dll"
		"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/release/libfbxsdk.dll"
        $<TARGET_FILE_DIR:HodEngine>)
else()

set(Libs
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysX_64"
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysXCommon_64"
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysXExtensions_static_64"
"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysXFoundation_64"
"../SDK/SDL/VisualC/x64/Debug/SDL2"
"../SDK/SDL/VisualC/x64/Debug/SDL2main"
"../SDK/SPIRV-Cross/Build/Debug/spirv-cross-core"
"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/debug/libfbxsdk.lib"
"d3d12.lib"
"DXGI.lib")

add_custom_command(TARGET HodEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "../SDK/SDL/VisualC/x64/Release/SDL2.dll"
		"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysX_64.dll"
		"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysXCommon_64.dll"
		"../SDK/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysXFoundation_64.dll"
		"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/debug/libfbxsdk.dll"
        $<TARGET_FILE_DIR:HodEngine>)

endif()

include_directories(${IncludeDirs})

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
