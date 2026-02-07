cmake_minimum_required(VERSION 3.10)

include(Global)

project(InputEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(InputEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::InputEditor ALIAS InputEditor)
set_target_properties(InputEditor PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(InputEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(InputEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(InputEditor
	PUBLIC Input Editor
)

target_compile_definitions(InputEditor PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(InputEditor PRIVATE HOD_INPUT_EDITOR_EXPORT)

install(TARGETS InputEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)
