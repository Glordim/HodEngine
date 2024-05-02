cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if (APPLE)
    project(HodApplication LANGUAGES CXX Swift)

    message(STATUS "Swift version : ${CMAKE_Swift_COMPILER_VERSION}")
    # Verify that we have a new enough compiler
    if("${CMAKE_Swift_COMPILER_VERSION}" VERSION_LESS 5.9)
    message(FATAL_ERROR "Bidirectional C++ Interop requires Swift 5.9 or greater. Have ${CMAKE_Swift_COMPILER_VERSION}")
    endif()

    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND
    NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    message(FATAL_ERROR "Project requires building with Clang.
    Have ${CMAKE_CXX_COMPILER_ID}")
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

    # Set up swiftrt.o and runtime library search paths
    include(InitializeSwift)
    # cmake/modules/AddSwift.cmake provides the function for creating the Swift to
    # C++ bridging header
    include(AddSwift)
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
	list(APPEND Srcs Main.swift)
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

include_directories(
	".."
	${Vulkan_INCLUDE_DIRS}
)

endif()

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

