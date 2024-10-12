cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Core)

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
target_precompile_headers(Core PRIVATE "pch.hpp")
set_property(TARGET Core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

if(APPLE)
	find_library(COREFOUNDATION_LIBRARY CoreFoundation)
endif()

if(ANDROID)
find_library(android-lib android)
target_link_libraries(Core ${android-lib} log)
endif()

if(WIN32)
	set(DebugLib "Dbghelp.lib")
endif()

target_link_libraries(Core ${COREFOUNDATION_LIBRARY} ${DebugLib})
target_compile_definitions(Core PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Core PRIVATE HOD_CORE_EXPORT)
