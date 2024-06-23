cmake_minimum_required(VERSION 3.4...3.27)

function(CollectSourceFiles ProjectSourceDir SourcesVar IncludesVar)

	cmake_parse_arguments(PARSED_ARGS "" "" "EXCLUDES" ${ARGN})

	if (APPLE)
		file(GLOB_RECURSE Sources
			"${ProjectSourceDir}/*.cpp"
			"${ProjectSourceDir}/*.mm"
			"${ProjectSourceDir}/*.hpp"
			"${ProjectSourceDir}/*.inl"
			"${ProjectSourceDir}/*.h"
		)
	else()
		file(GLOB_RECURSE Sources
			"${ProjectSourceDir}/*.cpp"
			"${ProjectSourceDir}/*.hpp"
			"${ProjectSourceDir}/*.inl"
			"${ProjectSourceDir}/*.h"
		)
	endif()

	if(PARSED_ARGS_EXCLUDES)
		foreach(Exclude ${PARSED_ARGS_EXCLUDES})
			message(Exclude:${Exclude})
			foreach(item ${Sources})
				if(item MATCHES ${Exclude})
					list(REMOVE_ITEM Sources ${item})
					message(Remove:${item})
				endif()
			endforeach()
		endforeach()
	endif()

	foreach(item ${Sources})
		if(${item} MATCHES ".+\\..+\\.cpp")
			list(REMOVE_ITEM Sources ${item})
		endif()
	endforeach()

	if(WIN32)
		file(GLOB_RECURSE PlatformSources
			"${ProjectSourceDir}/*.windows.cpp"
		)
	elseif(APPLE)
		file(GLOB_RECURSE PlatformSources
			"${ProjectSourceDir}/*.macos.cpp"
		)
	else()
		file(GLOB_RECURSE PlatformSources
			"${ProjectSourceDir}/*.linux.cpp"
		)
	endif()

	set(${SourcesVar} ${Sources} ${PlatformSources} PARENT_SCOPE)

	file(GLOB_RECURSE Includes
		"${ProjectSourceDir}/*.hpp"
		"${ProjectSourceDir}/*.inl"
		"${ProjectSourceDir}/*.h"
	)
	set(${IncludesVar} ${Includes} PARENT_SCOPE)
endfunction()

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

if(MSVC)
	#target_compile_options(HodEngine-Application PRIVATE /W4 /WX)
	#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
else()
	#target_compile_options(HodEngine-Application PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


if(APPLE)
set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED TRUE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 14)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT SET_UP_CONFIGURATIONS_DONE)
    set(SET_UP_CONFIGURATIONS_DONE TRUE)

    # No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator
    # Also no reason mess with CMAKE_BUILD_TYPE if it's a multiconfig generator.
    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(isMultiConfig)
        set(CMAKE_CONFIGURATION_TYPES "Game_Debug;Game_Release;Editor_Debug;Editor_Release;Server_Debug;Server_Release" CACHE STRING "" FORCE) 
    else()
        if(NOT CMAKE_BUILD_TYPE)
            message("Defaulting to Game_Release build.")
            set(CMAKE_BUILD_TYPE Game_Release CACHE STRING "" FORCE)
        endif()
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
        # set the valid options for cmake-gui drop-down list
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Game_Debug;Game_Release;Editor_Debug;Editor_Release;Server_Debug;Server_Release")
    endif()

	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -Werror -D_CRT_SECURE_NO_WARNINGS")
	elseif (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /D_CRT_SECURE_NO_WARNINGS")
	endif()

	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/bin)
	endforeach()

	set(COMPILE_DEFINITIONS_GAME_DEBUG
		HOD_GAME
		HOD_DEBUG
	)
	set(CMAKE_CXX_FLAGS_GAME_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	set(CMAKE_EXE_LINKER_FLAGS_GAME_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG})

	set(COMPILE_DEFINITIONS_GAME_RELEASE
		HOD_GAME
		HOD_RELEASE
	)
	set(CMAKE_CXX_FLAGS_GAME_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	set(CMAKE_EXE_LINKER_FLAGS_GAME_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE})

	set(COMPILE_DEFINITIONS_EDITOR_DEBUG
		HOD_EDITOR
		HOD_DEBUG
	)
	set(CMAKE_CXX_FLAGS_EDITOR_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	set(CMAKE_EXE_LINKER_FLAGS_EDITOR_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG})

	set(COMPILE_DEFINITIONS_EDITOR_RELEASE
		HOD_EDITOR
		HOD_RELEASE
	)
	set(CMAKE_CXX_FLAGS_EDITOR_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	set(CMAKE_EXE_LINKER_FLAGS_EDITOR_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE})

	set(COMPILE_DEFINITIONS_SERVER_DEBUG
		HOD_SERVER
		HOD_DEBUG
	)
	set(CMAKE_CXX_FLAGS_SERVER_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	set(CMAKE_EXE_LINKER_FLAGS_SERVER_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG})

	set(COMPILE_DEFINITIONS_SERVER_RELEASE
		HOD_SERVER
		HOD_RELEASE
	)
	set(CMAKE_CXX_FLAGS_SERVER_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	set(CMAKE_EXE_LINKER_FLAGS_SERVER_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
endif()

if(WIN32)
	set(PLATFORM_DEFINE "PLATFORM_WINDOWS")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
elseif(APPLE)
	if(IOS)
		set(PLATFORM_DEFINE "PLATFORM_IOS")
		set(RENDERER_DEFINE "RENDERER_METAL")
	else()
		set(PLATFORM_DEFINE "PLATFORM_MACOS")
		set(RENDERER_DEFINE "RENDERER_METAL")
	endif()
elseif(ANDROID)
	set(PLATFORM_DEFINE "PLATFORM_ANDROID")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
else()
	set(PLATFORM_DEFINE "PLATFORM_LINUX")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
endif()
