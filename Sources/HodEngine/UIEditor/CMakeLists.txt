cmake_minimum_required(VERSION 3.10)

include(Global)

project(UIEditor)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UIEditor ${LIB_TYPE} ${Srcs} ${Includes})
set_target_properties(UIEditor PROPERTIES FOLDER "HodEngine")
target_precompile_headers(UIEditor PRIVATE "Pch.hpp")
set_property(TARGET UIEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(UIEditor UI Editor)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Application_INCLUDES}
)

target_link_libraries(UIEditor
  UI
  Editor
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(UIEditor PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(UIEditor PRIVATE HOD_UI_EDITOR_EXPORT)
