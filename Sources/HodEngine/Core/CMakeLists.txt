cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Core)

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
)

add_library(Core OBJECT ${Srcs} ${Includes})

set(UUID_LIB "")

if(UNIX)
	set(UUID_LIB uuid)
endif(UNIX)

target_link_libraries(Core ${UUID_LIB})
target_compile_definitions(Core PUBLIC ${PLATFORM_DEFINE})
