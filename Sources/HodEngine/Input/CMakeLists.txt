cmake_minimum_required(VERSION 3.10)

if (APPLE)
	project(Input LANGUAGES CXX OBJCXX)
else()
	project(Input LANGUAGES CXX)
endif()

if (WIN32)
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)
else()
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES RawInput XInput)
endif()

assign_source_group(${Srcs}) # for VS

add_library(Input ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Input ALIAS Input)
set_target_properties(Input PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(Input PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Input PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

if (APPLE)
	target_link_libraries(Input PRIVATE "-framework GameController")
endif()
if (WIN32)
	target_link_libraries(Input PRIVATE WindowsHModular::WindowsHModular)
endif()

target_link_libraries(Input
	PUBLIC Core Math Window GameSystems
)

target_compile_definitions(Input PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Input PUBLIC HOD_INPUT_STATIC)
else()
	target_compile_definitions(Input PRIVATE HOD_INPUT_EXPORT)
endif()

hod_install_lib(Input)
