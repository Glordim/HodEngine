cmake_minimum_required(VERSION 3.10)

include(Global)

project(CoreEditor)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(CoreEditor ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(CoreEditor PRIVATE "Pch.hpp")
set_property(TARGET CoreEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(CoreEditor Editor)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Application_INCLUDES}
)

target_link_libraries(CoreEditor
  Editor
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(CoreEditor PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(CoreEditor PRIVATE HOD_CORE_EDITOR_EXPORT)
