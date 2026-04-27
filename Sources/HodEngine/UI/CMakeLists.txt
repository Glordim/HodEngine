cmake_minimum_required(VERSION 3.10)

project(UI LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UI ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UI ALIAS UI)
set_target_properties(UI PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(UI PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UI PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(UI
	PUBLIC Game Renderer
)

target_compile_definitions(UI PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(UI PUBLIC HOD_UI_STATIC)
else()
	target_compile_definitions(UI PRIVATE HOD_UI_EXPORT)
endif()

hod_install_lib(UI)
