cmake_minimum_required(VERSION 3.10)

include(Global)

project(UI)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UI ${LIB_TYPE} ${Srcs} ${Includes})
set_target_properties(UI PROPERTIES FOLDER "HodEngine")
target_precompile_headers(UI PRIVATE "Pch.hpp")
set_property(TARGET UI PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(UI Game)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Application_INCLUDES}
)

target_link_libraries(UI
  Game
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(UI PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(UI PRIVATE HOD_UI_EXPORT)
