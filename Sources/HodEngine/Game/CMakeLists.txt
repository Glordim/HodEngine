cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Game)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${box2d_INCLUDE_DIRS}
)

add_library(Game STATIC ${Srcs} ${Includes})
set_property(TARGET Game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

target_compile_definitions(Game PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HODENGINE_EXPORTS)
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HODENGINE_EXPORTS)
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HODENGINE_EXPORTS)
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HODENGINE_EXPORTS)
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HODENGINE_EXPORTS)
target_compile_definitions(Game PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HODENGINE_EXPORTS)

add_dependencies(Game Core ImGui Window Physics Renderer)

target_link_libraries(Game
	Core
	ImGui
	Window
	Physics
	Renderer
)
