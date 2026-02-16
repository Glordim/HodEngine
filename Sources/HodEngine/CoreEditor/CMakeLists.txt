cmake_minimum_required(VERSION 3.10)

include(ClangBuildAnalyzer)

project(CoreEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(CoreEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::CoreEditor ALIAS CoreEditor)
set_target_properties(CoreEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(CoreEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(CoreEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(CoreEditor
	PUBLIC Core Editor
)

target_compile_definitions(CoreEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_CORE_EDITOR_EXPORT
)

install(TARGETS CoreEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(CoreEditor)
