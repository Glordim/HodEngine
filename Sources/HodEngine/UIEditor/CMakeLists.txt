cmake_minimum_required(VERSION 3.10)

include(ClangBuildAnalyzer)

project(UIEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UIEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UIEditor ALIAS UIEditor)
set_target_properties(UIEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(UIEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UIEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(UIEditor
	PUBLIC UI Editor
)

target_compile_definitions(UIEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_UI_EDITOR_EXPORT
)

install(TARGETS UIEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(UIEditor)
