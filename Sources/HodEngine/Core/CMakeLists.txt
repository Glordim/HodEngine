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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
)

add_library(Core ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(Core PRIVATE "Pch.hpp")
set_property(TARGET Core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

if(APPLE)
	find_library(COREFOUNDATION_LIBRARY CoreFoundation)
endif()

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
endif()

if(ANDROID)
find_library(android-lib android)
target_link_libraries(Core ${android-lib} log)
endif()

if(WIN32)
	set(DebugLib "Dbghelp.lib")
endif()

if(PLATFORM_LINUX)
	target_link_libraries(Core uuid)
endif()

target_link_libraries(Core ${COREFOUNDATION_LIBRARY} ${COCOA_LIBRARY} ${DebugLib})
target_compile_definitions(Core PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Core PRIVATE HOD_CORE_EXPORT)
