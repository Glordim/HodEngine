cmake_minimum_required(VERSION 3.10)

include(Global)

project(TestsCore)

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

add_executable(TestsCore ${Srcs} ${Includes})
set_property(TARGET TestsCore PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

#add_dependencies(HodEngine-Application Core DebugLayer ImGui Physics Application Renderer Game)
#add_dependencies(HodEngine-Editor Core DebugLayer ImGui Physics Application Renderer Game)

#add_executable(HodEngine-Application ${Srcs})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestsCore)
#set_property(TARGET HodEngine-Application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "../Data")
#add_dependencies(HodEngine-Application Core DebugLayer ImGui Physics Application Renderer Game)

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
#find_package(Glm REQUIRED)
#find_package(Vulkan REQUIRED)

add_dependencies(TestsCore Core)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
)

target_link_libraries(TestsCore
	Core
	GTest::gtest_main
)

target_compile_definitions(TestsCore PUBLIC ${PLATFORM_DEFINE})

include(GoogleTest)
gtest_discover_tests(TestsCore)
