cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Core)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
)

add_library(Core STATIC ${Srcs} ${Includes})
target_precompile_headers(Core PRIVATE "pch.hpp")
set_property(TARGET Core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set(UUID_LIB "")

if(UNIX)
	if (APPLE)
		set (UUID_LIB "-framework CoreFoundation")
	else()
		set(UUID_LIB uuid)
	endif()
endif(UNIX)

target_link_libraries(Core ${UUID_LIB})
target_compile_definitions(Core PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HOD_EXPORT)
