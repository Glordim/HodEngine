cmake_minimum_required(VERSION 3.10)

set(HOD_PLATFORM_SUBDIR "platforms/${HOD_PLATFORM}")

set(HOD_BIN_DIR "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/bin")
set(HOD_LIB_DIR "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/lib")

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${HOD_BIN_DIR}/${OUTPUTCONFIG})
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${HOD_BIN_DIR}/${OUTPUTCONFIG})
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${HOD_LIB_DIR}/${OUTPUTCONFIG})
	endforeach()
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HOD_BIN_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${HOD_BIN_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HOD_LIB_DIR})
endif()

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "" FORCE)
endif()

macro(hod_install_lib TARGET_NAME)
	install(TARGETS ${TARGET_NAME}
		EXPORT HodEngineTargets
		LIBRARY DESTINATION "${HOD_PLATFORM_SUBDIR}/lib"
		ARCHIVE DESTINATION "${HOD_PLATFORM_SUBDIR}/lib"
		RUNTIME DESTINATION "${HOD_PLATFORM_SUBDIR}/bin"
	)
endmacro()

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
	if(HOD_PLATFORM_WINDOWS)
		list(REMOVE_ITEM PlatformDirs Windows)
	elseif(HOD_PLATFORM_MACOS)
		list(REMOVE_ITEM PlatformDirs MacOs)
	elseif(HOD_PLATFORM_LINUX)
		list(REMOVE_ITEM PlatformDirs Linux)
	elseif(HOD_PLATFORM_ANDROID)
		list(REMOVE_ITEM PlatformDirs Android)
	elseif(HOD_PLATFORM_IOS)
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
