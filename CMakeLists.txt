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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile;Retail")
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "")
set(CMAKE_EXE_LINKER_FLAGS_RETAIL "")

set(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

#set(CMAKE_SUPPRESS_REGENERATION true)

project(HodEngine) # Rename

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(WIN32)
	set(PLATFORM_DEFINE "PLATFORM_WINDOWS")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
elseif(APPLE)
	if(IOS)
		set(PLATFORM_DEFINE "PLATFORM_IOS")
		set(RENDERER_DEFINE "RENDERER_METAL")
	else()
		set(PLATFORM_DEFINE "PLATFORM_MAC")
		set(RENDERER_DEFINE "RENDERER_METAL")
	endif()
elseif(ANDROID)
	set(PLATFORM_DEFINE "PLATFORM_ANDROID")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
else()
	set(PLATFORM_DEFINE "PLATFORM_LINUX")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
endif()

add_subdirectory("Sources/HodEngine")
add_subdirectory("Sources/Application")
add_subdirectory("Sources/Editor")
