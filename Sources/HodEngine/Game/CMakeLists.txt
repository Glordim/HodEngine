cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Game)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
)

add_library(Game OBJECT ${Srcs} ${Includes})
set_property(TARGET Game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

target_compile_definitions(Game PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")

add_dependencies(Game Core ImGui Window Physics Renderer)

target_link_libraries(Game
	Core
	ImGui
	Window
	Physics
	Renderer
)
