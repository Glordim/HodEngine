cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Core)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE Includes
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
)

add_library(Core OBJECT ${Srcs} ${Includes})
set_property(TARGET Core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set(UUID_LIB "")

if(UNIX)
	set(UUID_LIB uuid)
endif(UNIX)

target_link_libraries(Core ${UUID_LIB})
target_compile_definitions(Core PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(Core PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")
