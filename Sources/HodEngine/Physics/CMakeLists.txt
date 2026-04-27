cmake_minimum_required(VERSION 3.10)

project(Physics LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Physics ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Physics ALIAS Physics)
set_target_properties(Physics PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(Physics PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Physics PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Physics
	PUBLIC Core Math
	PRIVATE Box2d::Box2d
)

target_compile_definitions(Physics PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Physics PUBLIC HOD_PHYSICS_STATIC)
else()
	target_compile_definitions(Physics PRIVATE HOD_PHYSICS_EXPORT)
endif()

hod_install_lib(Physics)
