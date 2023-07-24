cmake_minimum_required(VERSION 3.4...3.27)

set(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(HodEngine) # Rename

add_subdirectory("Sources/HodEngine/Core")
add_subdirectory("Sources/HodEngine/Window")

#add_subdirectory("Sources/HodEngine")
#add_subdirectory("Sources/GameApplication")
#add_subdirectory("Sources/EditorApplication")
