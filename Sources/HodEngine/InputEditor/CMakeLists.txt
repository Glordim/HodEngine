cmake_minimum_required(VERSION 3.10)

include(ClangBuildAnalyzer)

project(InputEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(InputEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::InputEditor ALIAS InputEditor)
set_target_properties(InputEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(InputEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(InputEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(InputEditor
	PUBLIC Input Editor
)

target_compile_definitions(InputEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_INPUT_EDITOR_EXPORT
)

install(TARGETS InputEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(InputEditor)
