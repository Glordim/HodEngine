cmake_minimum_required(VERSION 3.10)

include(ClangBuildAnalyzer)

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
	PUBLIC Core Window
)

target_compile_definitions(Input
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_INPUT_EXPORT
)

install(TARGETS Input
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(Input)
