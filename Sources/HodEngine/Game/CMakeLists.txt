cmake_minimum_required(VERSION 3.10)

include(Global)
include(ClangBuildAnalyzer)

project(Game LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Game ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Game ALIAS Game)
set_target_properties(Game PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(Game PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Game PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_compile_definitions(Game PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Game PRIVATE HOD_GAME_EXPORT)

target_link_libraries(Game
	PUBLIC Core Window Physics Renderer
)

install(TARGETS Game
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(Game)
