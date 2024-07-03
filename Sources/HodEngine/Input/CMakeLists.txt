cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Input)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(Input OBJECT ${Srcs} ${Includes})
set_property(TARGET Input PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Input Core Window Application)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)
get_target_property(Application_INCLUDES Application INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Application_INCLUDES}
)

target_link_libraries(Input
  Core
  Application
  Window
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(Input PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Input PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HODENGINE_EXPORTS)
target_compile_definitions(Input PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HODENGINE_EXPORTS)
target_compile_definitions(Input PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HODENGINE_EXPORTS)
target_compile_definitions(Input PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HODENGINE_EXPORTS)
target_compile_definitions(Input PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HODENGINE_EXPORTS)
target_compile_definitions(Input PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HODENGINE_EXPORTS)
