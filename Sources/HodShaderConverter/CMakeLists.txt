cmake_minimum_required(VERSION 3.10)

project(HodShaderConverter)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE Includes
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

assign_source_group(${Srcs}) # for VS

add_executable(HodShaderConverter ${Srcs} ${Includes})
set_property(TARGET HodShaderConverter PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

include_directories(
	".."
)

#add_dependencies(HodShaderConverter Core)

#target_link_libraries(HodShaderConverter
#	Core
#)

target_compile_definitions(HodShaderConverter PUBLIC ${HOD_PLATFORM_DEFINE})
