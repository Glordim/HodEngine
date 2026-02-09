cmake_minimum_required(VERSION 3.10)

include(Global)
include(ClangBuildAnalyzer)

project(Physics LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Physics ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Physics ALIAS Physics)
set_target_properties(Physics PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(Physics PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Physics PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Physics
	PUBLIC Core
	PRIVATE Box2d::Box2d
)

target_compile_definitions(Physics PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Physics PRIVATE HOD_PHYSICS_EXPORT)

install(TARGETS Physics
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(Physics)
