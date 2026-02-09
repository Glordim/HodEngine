cmake_minimum_required(VERSION 3.10)

include(Global)
include(ClangBuildAnalyzer)

project(UI LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UI ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UI ALIAS UI)
set_target_properties(UI PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(UI PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UI PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(UI
	PUBLIC Game Renderer
)

target_compile_definitions(UI PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(UI PRIVATE HOD_UI_EXPORT)

install(TARGETS UI
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(UI)
