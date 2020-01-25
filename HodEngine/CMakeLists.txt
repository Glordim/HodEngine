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

project(HodEngine)
#add_subdirectory(Core)
add_subdirectory(Physic)
	
file(GLOB_RECURSE Srcs
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
		"${PROJECT_SOURCE_DIR}/src/*.inl"
        )
	
assign_source_group(${Srcs})

add_executable(HodEngine ${Srcs})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodEngine)
set_property(TARGET HodEngine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "../../Data")

file(GLOB_RECURSE SrcGame
        "${PROJECT_SOURCE_DIR}/src/Game/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/Game/*.h"
		"${PROJECT_SOURCE_DIR}/src/Game/*.inl"
        )

add_library(Game ${SrcGame})

file(GLOB_RECURSE SrcRenderer
        "${PROJECT_SOURCE_DIR}/src/Renderer/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/Renderer/*.h"
		"${PROJECT_SOURCE_DIR}/src/Renderer/*.inl"
        )

add_library(Renderer ${SrcRenderer})

#if(MSVC)
#	target_compile_options(HodEngine PRIVATE /W4 /WX)
#else()
#	target_compile_options(HodEngine PRIVATE -Wall -Wextra -pedantic -Werror)
#endif()

include_directories(
"${PROJECT_SOURCE_DIR}"
"${PROJECT_SOURCE_DIR}/src"
"../SDK/glm"
"../SDK/SDL/include"
"../SDK/tinyobjloader"
"../SDK/stb"
"../SDK/SPIRV-Cross"
"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/include"
)

target_link_libraries(HodEngine Core Physic
optimized "../../SDK/Build/PhysX_lib/release/PhysX_64" debug "../../SDK/Build/PhysX_lib/debug/PhysX_64"
optimized "../../SDK/Build/PhysX_lib/release/PhysXCommon_64" debug "../../SDK/Build/PhysX_lib/debug/PhysXCommon_64"
optimized "../../SDK/Build/PhysX_lib/release/PhysXExtensions_static_64" debug "../../SDK/Build/PhysX_lib/debug/PhysXExtensions_static_64"
optimized "../../SDK/Build/PhysX_lib/release/PhysXFoundation_64" debug "../../SDK/Build/PhysX_lib/debug/PhysXFoundation_64"
optimized "../../SDK/Build/SDL/Release/SDL2" debug "../../SDK/Build/SDL/Debug/SDL2d"
optimized "../../SDK/Build/SDL/Release/SDL2main" debug "../../SDK/Build/SDL/Debug/SDL2maind"
optimized "../../SDK/Build/SPIRV-Cross/Release/spirv-cross-core" debug "../../SDK/Build/SPIRV-Cross/Debug/spirv-cross-core"
optimized "../../SDK/Build/tinyobjloader/Release/tinyobjloader" debug "../../SDK/Build/tinyobjloader/Debug/tinyobjloader"
optimized "../../SDK/Build/glm/glm/Release/glm_static" debug "../../SDK/Build/glm/glm/Debug/glm_static"
optimized "C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/release/libfbxsdk.lib" debug "C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/debug/libfbxsdk.lib"
optimized "d3d12.lib" debug "d3d12.lib"
optimized "DXGI.lib" debug "DXGI.lib"
)

add_custom_command(TARGET HodEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../../SDK/Build/SDL/Debug/SDL2d.dll">$<$<CONFIG:release>:"../../SDK/Build/SDL/Release/SDL2.dll"> $<TARGET_FILE_DIR:HodEngine>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../../SDK/Build/PhysX_lib/debug/PhysX_64.dll">$<$<CONFIG:release>:"../../SDK/Build/PhysX_lib/release/PhysX_64.dll"> $<TARGET_FILE_DIR:HodEngine>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../../SDK/Build/PhysX_lib/debug/PhysXCommon_64.dll">$<$<CONFIG:release>:"../../SDK/Build/PhysX_lib/release/PhysXCommon_64.dll"> $<TARGET_FILE_DIR:HodEngine>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../../SDK/Build/PhysX_lib/debug/PhysXFoundation_64.dll">$<$<CONFIG:release>:"../../SDK/Build/PhysX_lib/release/PhysXFoundation_64.dll"> $<TARGET_FILE_DIR:HodEngine>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/debug/libfbxsdk.dll">$<$<CONFIG:release>:"C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/release/libfbxsdk.dll"> $<TARGET_FILE_DIR:HodEngine>
)

find_package(Vulkan)
if (Vulkan_FOUND)
  # Une fois la dépendance trouvée, nous l'incluons au projet
  target_include_directories(HodEngine PUBLIC ${Vulkan_INCLUDE_DIRS})
  target_link_libraries (HodEngine ${Vulkan_LIBRARIES})
else ()
  # Sinon, nous affichons un message
  message(FATAL_ERROR "Vulkan not found")
endif ()
