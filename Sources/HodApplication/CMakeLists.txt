cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if (APPLE)
    project(HodApplication LANGUAGES CXX OBJCXX)
else()
    project(HodApplication LANGUAGES CXX)
endif()

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

if (APPLE)
	add_executable(HodApplication ${Srcs} ${Includes})
	target_include_directories(HodApplication PRIVATE ${PROJECT_SOURCE_DIR})
else()
	add_executable(HodApplication ${Srcs} ${Includes})
	set_property(TARGET HodApplication PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodApplication)

endif()

include_directories(
	".."
)

add_dependencies(HodApplication
	Application
)

target_link_libraries(HodApplication
	Application
)

target_compile_definitions(HodApplication PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})

