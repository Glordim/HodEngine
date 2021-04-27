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

set (SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK")

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
"${SDK_DIR}/glm"
"${SDK_DIR}/rapidjson/include"
)

target_link_libraries(HodEngine
Core
DebugLayer
ImGui
Physics
Application
Renderer
Game
)

add_custom_command(TARGET HodEngine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${SDK_DIR}/Build/SDL/Debug/SDL2d.dll">$<$<CONFIG:release>:"${SDK_DIR}/Build/SDL/Release/SDL2.dll"> $<TARGET_FILE_DIR:HodEngine>
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
