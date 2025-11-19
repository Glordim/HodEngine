cmake_minimum_required(VERSION 3.10)

include(Global)

project(Game)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${box2d_INCLUDE_DIRS}
)

add_library(Game ${LIB_TYPE} ${Srcs} ${Includes})
set_target_properties(Game PROPERTIES FOLDER "HodEngine")
target_precompile_headers(Game PRIVATE "Pch.hpp")
set_property(TARGET Game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

target_compile_definitions(Game PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Game PRIVATE HOD_GAME_EXPORT)

add_dependencies(Game Core Window Physics Renderer)

target_link_libraries(Game
	Core
	Window
	Physics
	Renderer
)
