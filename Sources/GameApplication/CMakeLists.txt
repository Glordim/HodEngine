cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(GameApplication)

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
target_compile_definitions(GameApplication PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(GameApplication PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(GameApplication PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(GameApplication PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(GameApplication PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(GameApplication PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")
