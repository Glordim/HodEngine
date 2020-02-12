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
if (NOT TARGET Core)
add_subdirectory(HodEngine/Core)
endif()
if (NOT TARGET DebugLayer)
add_subdirectory(HodEngine/DebugLayer)
endif()  
if (NOT TARGET ImGui)
add_subdirectory(HodEngine/ImGui)
endif() 
if (NOT TARGET Physics)
add_subdirectory(HodEngine/Physics)
endif()
if (NOT TARGET Application)
add_subdirectory(HodEngine/Application)
endif() 
if (NOT TARGET Renderer)
add_subdirectory(HodEngine/Renderer)
endif() 
if (NOT TARGET Game)
add_subdirectory(HodEngine/Game)
endif()
	
file(GLOB_RECURSE Srcs
        "${PROJECT_SOURCE_DIR}/Test/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/Test/src/*.h"
		"${PROJECT_SOURCE_DIR}/Test/src/*.inl"
        )
	
assign_source_group(${Srcs})

add_executable(HodEngine ${Srcs})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodEngine)
set_property(TARGET HodEngine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "../Data")

add_dependencies(HodEngine Core DebugLayer ImGui Physics Application Renderer Game)

#if(MSVC)
#	target_compile_options(HodEngine PRIVATE /W4 /WX)
#else()
#	target_compile_options(HodEngine PRIVATE -Wall -Wextra -pedantic -Werror)
#endif()

include_directories(
"${PROJECT_SOURCE_DIR}"
"${PROJECT_SOURCE_DIR}/src"
"HodEngine"
"SDK/glm"
)

target_link_libraries(HodEngine
optimized "HodEngine/Core/Release/Core" debug "HodEngine/Core/Debug/Core"
optimized "HodEngine/Game/Release/Game" debug "HodEngine/Game/Debug/Game"
optimized "HodEngine/Renderer/Release/Renderer" debug "HodEngine/Renderer/Debug/Renderer"
optimized "HodEngine/Application/Release/Application" debug "HodEngine/Application/Debug/Application"
optimized "HodEngine/Physics/Release/Physics" debug "HodEngine/Physics/Debug/Physics"
optimized "HodEngine/ImGui/Release/ImGui" debug "HodEngine/ImGui/Debug/ImGui"
optimized "HodEngine/DebugLayer/Release/DebugLayer" debug "HodEngine/DebugLayer/Debug/DebugLayer"
optimized "../SDK/Build/SDL/Release/SDL2" debug "../SDK/Build/SDL/Debug/SDL2d"
optimized "../SDK/Build/SDL/Release/SDL2main" debug "../SDK/Build/SDL/Debug/SDL2maind"
optimized "../SDK/Build/SPIRV-Cross/Release/spirv-cross-core" debug "../SDK/Build/SPIRV-Cross/Debug/spirv-cross-core"
optimized "../SDK/Build/tinyobjloader/Release/tinyobjloader" debug "../SDK/Build/tinyobjloader/Debug/tinyobjloader"
optimized "../SDK/Build/glm/glm/Release/glm_static" debug "../SDK/Build/glm/glm/Debug/glm_static"
optimized "../SDK/Build/PhysX_lib/release/PhysX_64" debug "../SDK/Build/PhysX_lib/debug/PhysX_64"
optimized "../SDK/Build/PhysX_lib/release/PhysXCommon_64" debug "../SDK/Build/PhysX_lib/debug/PhysXCommon_64"
optimized "../SDK/Build/PhysX_lib/release/PhysXExtensions_static_64" debug "../SDK/Build/PhysX_lib/debug/PhysXExtensions_static_64"
optimized "../SDK/Build/PhysX_lib/release/PhysXFoundation_64" debug "../SDK/Build/PhysX_lib/debug/PhysXFoundation_64"
optimized "C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/release/libfbxsdk.lib" debug "C:/Program Files/Autodesk/FBX/FBX SDK/2019.2/lib/vs2017/x64/debug/libfbxsdk.lib"
optimized "d3d12.lib" debug "d3d12.lib"
optimized "DXGI.lib" debug "DXGI.lib"
)

add_custom_command(TARGET HodEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../SDK/Build/SDL/Debug/SDL2d.dll">$<$<CONFIG:release>:"../SDK/Build/SDL/Release/SDL2.dll"> $<TARGET_FILE_DIR:HodEngine>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../SDK/Build/PhysX_lib/debug/PhysX_64.dll">$<$<CONFIG:release>:"../SDK/Build/PhysX_lib/release/PhysX_64.dll"> $<TARGET_FILE_DIR:HodEngine>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../SDK/Build/PhysX_lib/debug/PhysXCommon_64.dll">$<$<CONFIG:release>:"../SDK/Build/PhysX_lib/release/PhysXCommon_64.dll"> $<TARGET_FILE_DIR:HodEngine>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"../SDK/Build/PhysX_lib/debug/PhysXFoundation_64.dll">$<$<CONFIG:release>:"../SDK/Build/PhysX_lib/release/PhysXFoundation_64.dll"> $<TARGET_FILE_DIR:HodEngine>
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
