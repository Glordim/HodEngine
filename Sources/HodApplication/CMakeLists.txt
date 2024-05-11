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

#add_dependencies(HodEngine-Application Core DebugLayer ImGui Physics Application Renderer Game)
#add_dependencies(HodEngine-Editor Core DebugLayer ImGui Physics Application Renderer Game)

#add_executable(HodEngine-Application ${Srcs})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodApplication)
#set_property(TARGET HodEngine-Application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "../Data")
#add_dependencies(HodEngine-Application Core DebugLayer ImGui Physics Application Renderer Game)

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
#find_package(Glm REQUIRED)
#find_package(Vulkan REQUIRED)

endif()

include_directories(
	".."
)

add_dependencies(HodApplication HodEngine)

target_link_libraries(HodApplication
	HodEngine
)

target_compile_definitions(HodApplication PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(HodApplication PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(HodApplication PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(HodApplication PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(HodApplication PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(HodApplication PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(HodApplication PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")

