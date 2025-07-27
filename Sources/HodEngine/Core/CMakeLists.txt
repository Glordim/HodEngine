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
set_target_properties(Core PROPERTIES FOLDER "HodEngine")
target_precompile_headers(Core PRIVATE "Pch.hpp")
set_property(TARGET Core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

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

target_link_libraries(Core PRIVATE ${PLATFORM_LIBS})

target_compile_definitions(Core PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Core PRIVATE HOD_CORE_EXPORT)
