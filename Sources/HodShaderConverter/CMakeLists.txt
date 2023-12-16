cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(HodShaderConverter)

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

add_executable(HodShaderConverter ${Srcs} ${Includes})
set_property(TARGET HodShaderConverter PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

include_directories(
	".."
)

add_dependencies(HodShaderConverter Core)

target_link_libraries(HodShaderConverter
	Core
)

target_compile_definitions(HodShaderConverter PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(HodShaderConverter PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(HodShaderConverter PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(HodShaderConverter PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(HodShaderConverter PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(HodShaderConverter PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(HodShaderConverter PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")
				