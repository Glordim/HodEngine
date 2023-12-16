cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(EditorApplication)

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

add_executable(EditorApplication ${Srcs} ${Includes})
set_property(TARGET EditorApplication PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

include_directories(
	".."
	${Glm_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}
)

add_dependencies(EditorApplication Engine)

target_link_libraries(EditorApplication
	Engine
)

target_compile_definitions(EditorApplication PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(EditorApplication PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(EditorApplication PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(EditorApplication PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(EditorApplication PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(EditorApplication PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(EditorApplication PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")

add_custom_command(
        TARGET EditorApplication  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/EditorData
                $<TARGET_FILE_DIR:EditorApplication>/Data)
				