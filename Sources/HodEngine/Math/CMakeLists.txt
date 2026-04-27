cmake_minimum_required(VERSION 3.10)

project(Math LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Math ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Math ALIAS Math)

set_target_properties(Math PROPERTIES
	FOLDER "HodEngine"
)
target_include_directories(Math PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Math PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Math
	PUBLIC HodEngine::Core
)

target_compile_definitions(Math
	PUBLIC ${HOD_PLATFORM_DEFINE}
)

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Math PUBLIC HOD_MATH_STATIC)
else()
	target_compile_definitions(Math PRIVATE HOD_MATH_EXPORT)
endif()

hod_install_lib(Math)
