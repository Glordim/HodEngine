cmake_minimum_required(VERSION 3.4...3.27)

include(FetchContent)

FetchContent_Declare(
  stb
  URL https://github.com/nothings/stb/archive/refs/heads/master.zip)

  
#FetchContent_Declare(
#  box2d
#  URL https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.zip)

FetchContent_MakeAvailable(stb)
#FetchContent_Populate(box2d)
message(STATUS "stb is available in " ${stb_SOURCE_DIR})

FetchContent_Declare(
  portable_file_dialogs
  URL https://github.com/samhocevar/portable-file-dialogs/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(portable_file_dialogs)
message(STATUS "portable_file_dialogs is available in " ${portable_file_dialogs_SOURCE_DIR})

include(ExternalProject)
ExternalProject_Add(box2d
  URL https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/box2d-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/box2d-build
  CMAKE_ARGS
    -DBOX2D_BUILD_UNIT_TESTS=OFF
    -DBOX2D_BUILD_TESTBED=OFF
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/box2d-build --config Release
  INSTALL_COMMAND ""
)

set(box2d_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/box2d-src/include)
if(WIN32)
  set(box2d_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/box2d-build/bin/Release/box2d.lib)
elseif(UNIX)
  set(box2d_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/box2d-build/bin/libbox2d.a)
endif()

message(STATUS "box2d is available in " ${box2d_INCLUDE_DIRS})
message(STATUS "box2d is available in " ${box2d_LIBRARIES})

set(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Hod) # Rename

add_subdirectory("Sources/HodEngine/Core")

add_subdirectory("Sources/HodShaderConverter")

add_subdirectory("Sources/HodEngine/Window")
add_subdirectory("Sources/HodEngine/Renderer")
add_subdirectory("Sources/HodEngine/ImGui")
add_subdirectory("Sources/HodEngine/Physics")
add_subdirectory("Sources/HodEngine/Game")
add_subdirectory("Sources/HodEngine/Editor")
add_subdirectory("Sources/HodEngine/Application")

add_subdirectory("Sources/HodEngine")
add_subdirectory("Sources/HodApplication")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory("Sources/Tests/Core")
