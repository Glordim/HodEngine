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

project(HodEngine-Application)

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

if (NOT TARGET HodEngine-Editor)
	add_subdirectory(HodEngine-Editor)
endif()
	
file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/Test/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/Test/src/*.h"
	"${PROJECT_SOURCE_DIR}/Test/src/*.inl"
)
assign_source_group(${Srcs})

add_executable(HodEngine-Application ${Srcs})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodEngine-Application)
set_property(TARGET HodEngine-Application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "../Data")

add_dependencies(HodEngine-Application Core DebugLayer ImGui Physics Application Renderer Game)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
find_package(Glm REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(RapidJSON REQUIRED)

#if(MSVC)
#	target_compile_options(HodEngine-Application PRIVATE /W4 /WX)
#else()
#	target_compile_options(HodEngine-Application PRIVATE -Wall -Wextra -pedantic -Werror)
#endif()

include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
	"HodEngine"
	${Glm_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}
	${RapidJSON_INCLUDE_DIRS}
)

target_link_libraries(HodEngine-Application
	Core
	DebugLayer
	ImGui
	Physics
	Application
	Renderer
	Game
)

if (WIN32)
	add_custom_command(TARGET HodEngine-Application POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${SDK_DIR}/Build/SDL/Debug/SDL2d.dll">$<$<CONFIG:release>:"${SDK_DIR}/Build/SDL/Release/SDL2.dll"> $<TARGET_FILE_DIR:HodEngine-Application>
	)
endif (WIN32)
