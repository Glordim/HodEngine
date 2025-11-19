cmake_minimum_required(VERSION 3.10)

include(Global)

if (APPLE)
    project(Input LANGUAGES CXX OBJCXX)
else()
    project(Input LANGUAGES CXX)
endif()

if (WIN32)
  CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)
else()
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES RawInput XInput)
endif()

add_library(Input ${LIB_TYPE} ${Srcs} ${Includes})
set_target_properties(Input PROPERTIES FOLDER "HodEngine")
target_precompile_headers(Input PRIVATE "Pch.hpp")
set_property(TARGET Input PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Input Core Window)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Application_INCLUDES}
)

if (APPLE)
  set(PLATFORM_LIBS "-framework GameController")
endif()

target_link_libraries(Input PUBLIC
  Core
  Window
)

target_link_libraries(Input PRIVATE
  ${PLATFORM_LIBS}
  WindowsHModular::WindowsHModular
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(Input PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Input PRIVATE HOD_INPUT_EXPORT)
