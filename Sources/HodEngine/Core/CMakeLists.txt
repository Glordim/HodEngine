cmake_minimum_required(VERSION 3.10)

include(Global)

if (APPLE)
	project(Core LANGUAGES CXX OBJCXX)
else()
	project(Core LANGUAGES CXX)
endif()

if (ANDROID)
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES FileSystemWatcher)
else()
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)
endif()

assign_source_group(${Srcs}) # for VS

add_library(Core ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Core ALIAS Core)

set_target_properties(Core PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)
target_include_directories(Core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Core PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

if (APPLE)
	find_library(COREFOUNDATION_LIBRARY CoreFoundation)
	find_library(COCOA_LIBRARY Cocoa)
	set(PLATFORM_LIBS ${COREFOUNDATION_LIBRARY} ${COCOA_LIBRARY})
elseif(ANDROID)
	find_library(ANDROID_LIBRARY android)
	set(PLATFORM_LIBS ${ANDROID_LIBRARY} log)
elseif(WIN32)
	set(PLATFORM_LIBS "Dbghelp.lib")
else()
	set(PLATFORM_LIBS uuid)
endif()

target_link_libraries(Core
	PRIVATE ${PLATFORM_LIBS} WindowsHModular::WindowsHModular
	PUBLIC Fmt::Fmt
)

target_compile_definitions(Core
	PUBLIC ${PLATFORM_DEFINE}
	PRIVATE HOD_CORE_EXPORT
)

install(TARGETS Core
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

option(ENABLE_SUPERLUMINAL_PROFILER "Enable Superluminal profiler" OFF)
if(ENABLE_SUPERLUMINAL_PROFILER)
	list(APPEND CMAKE_MODULE_PATH "C:/Program Files/Superluminal/Performance/API")
	find_package(SuperluminalAPI REQUIRED)
	target_link_libraries(Core PRIVATE ${SuperluminalAPI_LIBS_RELEASE})
	target_include_directories(Core PRIVATE ${SuperluminalAPI_INCLUDE_DIRS})
	target_compile_definitions(Core PRIVATE HOD_USE_SUPERLIMINAL_PROFILER)
endif()
