cmake_minimum_required(VERSION 3.4...3.27)

if(WIN32)
	set(PLATFORM_DEFINE "PLATFORM_WINDOWS")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
	set(PLATFORM_WINDOWS True)
elseif(APPLE)
	if(IOS)
		set(PLATFORM_DEFINE "PLATFORM_IOS")
		set(RENDERER_DEFINE "RENDERER_METAL")
		set(PLATFORM_IOS True)
	else()
		set(PLATFORM_DEFINE "PLATFORM_MACOS")
		set(RENDERER_DEFINE "RENDERER_METAL")
		set(PLATFORM_MACOS True)
	endif()
elseif(ANDROID)
	set(PLATFORM_DEFINE "PLATFORM_ANDROID")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
	set(PLATFORM_ANDROID True)
else()
	set(PLATFORM_DEFINE "PLATFORM_LINUX")
	set(RENDERER_DEFINE "RENDERER_VULKAN")
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(PLATFORM_LINUX True)
endif()

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
			#message(Exclude:${Exclude})
			foreach(item ${Sources})
				if(item MATCHES ${Exclude})
					list(REMOVE_ITEM Sources ${item})
					#message(Remove:${item})
				endif()
			endforeach()
		endforeach()
	endif()

	foreach(item ${Sources})
		if(${item} MATCHES ".+\\..+\\.cpp")
			list(REMOVE_ITEM Sources ${item})
		endif()
	endforeach()

	set(PlatformDirs Windows MacOs Linux Android IOS)
	if(PLATFORM_WINDOWS)
		list(REMOVE_ITEM PlatformDirs Windows)
	elseif(PLATFORM_MACOS)
		list(REMOVE_ITEM PlatformDirs MacOs)
	elseif (PLATFORM_LINUX)
		list(REMOVE_ITEM PlatformDirs Linux)
	elseif (PLATFORM_ANDROID)
		list(REMOVE_ITEM PlatformDirs Android)
	elseif(PLATFORM_IOS)
		list(REMOVE_ITEM PlatformDirs IOS)
	endif()

	foreach(item ${Sources})
		set(shouldRemove FALSE)
		string(TOLOWER ${item} lower_item)
		
		foreach(platform ${PlatformDirs})
			string(TOLOWER ${platform} lower_platform)
			if(${lower_item} MATCHES "(/|^)(${lower_platform})(/|$)")
				set(shouldRemove TRUE)
				break()
			endif()
		endforeach()

		if(shouldRemove)
			#message(Exclude:${item})
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
	elseif (ANDROID)
		file(GLOB_RECURSE PlatformSources
			"${ProjectSourceDir}/*.android.cpp"
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

function(add_force_load_libs target)
    if (APPLE)
		foreach(lib IN LISTS ARGN)
			# Add the force_load flag for each library
			target_link_libraries(${target} PRIVATE ${lib})
			target_link_options(${target} PRIVATE "-Wl,-force_load,$<TARGET_FILE:${lib}>")
		endforeach()
    elseif (UNIX)
        # For Unix-like systems (Linux)
        set_property(TARGET ${target} APPEND PROPERTY LINK_FLAGS "-Wl,--whole-archive -Wl,--start-group")
        foreach(lib IN LISTS ARGN)
            target_link_libraries(${target} PRIVATE ${lib})
        endforeach()
        set_property(TARGET ${target} APPEND PROPERTY LINK_FLAGS "-Wl,--end-group -Wl,--no-whole-archive")
    else()
		foreach(lib IN LISTS ARGN)
            target_link_libraries(${target} PRIVATE ${lib})
        endforeach()
        #message(FATAL_ERROR "force_load is not supported on this platform.")
    endif()
endfunction()

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
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT SET_UP_CONFIGURATIONS_DONE)
    set(SET_UP_CONFIGURATIONS_DONE TRUE)

    # No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator
    # Also no reason mess with CMAKE_BUILD_TYPE if it's a multiconfig generator.
    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(isMultiConfig)
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 
    else()
        if(NOT CMAKE_BUILD_TYPE)
            message("Defaulting to Release build.")
            set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
        endif()
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
        # set the valid options for cmake-gui drop-down list
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release")
    endif()

	if (MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
		set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
	endif()

	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -Wno-nullability-completeness -Werror -D_CRT_SECURE_NO_WARNINGS")
	elseif (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /D_CRT_SECURE_NO_WARNINGS /wd4251")
	else()
		message("Unknown compiler: " ${CMAKE_CXX_COMPILER})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -Wno-nullability-completeness -Werror -D_CRT_SECURE_NO_WARNINGS")
	endif()

	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
		set(CMAKE_PDB_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
	endforeach()
endif()
