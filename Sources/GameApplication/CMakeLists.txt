cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(GameApplication)

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

add_executable(GameApplication ${Srcs} ${Includes})
set_property(TARGET GameApplication PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

#add_dependencies(HodEngine-Application Core DebugLayer ImGui Physics Application Renderer Game)
#add_dependencies(HodEngine-Editor Core DebugLayer ImGui Physics Application Renderer Game)

#add_executable(HodEngine-Application ${Srcs})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GameApplication)
#set_property(TARGET HodEngine-Application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "../Data")
#add_dependencies(HodEngine-Application Core DebugLayer ImGui Physics Application Renderer Game)

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
#find_package(Glm REQUIRED)
#find_package(Vulkan REQUIRED)

include_directories(
	".."
	${Glm_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}
)

add_dependencies(GameApplication Engine)

target_link_libraries(GameApplication
	Engine
)

target_compile_definitions(GameApplication PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
