cmake_minimum_required(VERSION 3.10)

include(ClangBuildAnalyzer)

project(GameEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(GameEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::GameEditor ALIAS GameEditor)
set_target_properties(GameEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(GameEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(GameEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(GameEditor
	PUBLIC Game Editor
)

target_compile_definitions(GameEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_GAME_EDITOR_EXPORT
)

install(TARGETS GameEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(GameEditor)
