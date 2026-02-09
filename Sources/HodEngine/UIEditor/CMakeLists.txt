cmake_minimum_required(VERSION 3.10)

include(Global)
include(ClangBuildAnalyzer)

project(UIEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UIEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UIEditor ALIAS UIEditor)
set_target_properties(UIEditor PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(UIEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UIEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(UIEditor
	PUBLIC UI Editor
)

target_compile_definitions(UIEditor PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(UIEditor PRIVATE HOD_UI_EDITOR_EXPORT)

install(TARGETS UIEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(UIEditor)
