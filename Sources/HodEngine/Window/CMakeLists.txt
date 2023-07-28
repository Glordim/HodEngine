cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Window)

if(UNIX)
	find_package(Wayland REQUIRED)
endif()

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.inl"
)

file(GLOB_RECURSE Includes
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.inl"
)

assign_source_group(${Srcs}) # for VS

add_library(Window OBJECT ${Srcs} ${Includes})
set_property(TARGET Window PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Window Core)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${Core_INCLUDES}
	${WAYLAND_CLIENT_INCLUDE_DIR}
)

target_link_libraries(Window
	Core
	${WAYLAND_CLIENT_LIBRARIES}
)

target_compile_definitions(Window PUBLIC ${PLATFORM_DEFINE})
