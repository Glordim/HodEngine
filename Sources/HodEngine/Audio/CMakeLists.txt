cmake_minimum_required(VERSION 3.10)

include(Global)
include(ClangBuildAnalyzer)

if (APPLE)
	project(Window LANGUAGES CXX OBJCXX)
else()
	project(Window LANGUAGES CXX)
endif()

if (APPLE)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES Win32 Android Linux)
elseif(ANDROID)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES MacOs Win32 Linux)
elseif(WIN32)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES MacOs Android Linux)
else() # Linux
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES Win32 Android MacOs)
endif()

assign_source_group(${Srcs}) # for VS

add_library(Audio ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Audio ALIAS Audio)
set_target_properties(Audio PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(Audio PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Audio PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Audio
	PUBLIC Core
)

target_compile_definitions(Audio PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Audio PRIVATE HOD_AUDIO_EXPORT)

install(TARGETS Audio
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(Audio)
