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

set(box2d_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/box2d-src/include)
if(WIN32)
  set(box2d_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/box2d-build/src/Release/box2d.lib)
elseif(UNIX)
  set(box2d_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/box2d-build/src/Release/libbox2d.a)
endif()

include(ExternalProject)
ExternalProject_Add(box2d
  URL https://github.com/erincatto/box2d/archive/refs/tags/v3.0.0.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/box2d-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/box2d-build
  CMAKE_ARGS
    -DBOX2D_SAMPLES=OFF
    -DBOX2D_VALIDATE=OFF
    -DBOX2D_UNIT_TESTS=OFF
    -DCMAKE_OSX_DEPLOYMENT_TARGET=14 #TODO only on MacOs platform
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/box2d-build --config Release
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${box2d_LIBRARIES}
)

message(STATUS "box2d is available in " ${box2d_INCLUDE_DIRS})
message(STATUS "box2d is available in " ${box2d_LIBRARIES})

set(msdfgen_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/msdfgen-src/include)
if(WIN32)
  set(msdfgen_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/msdfgen-build/Release/msdfgen-core.lib)
elseif(UNIX)
  set(msdfgen_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/msdfgen-build/Release/libmsdfgen-core.a)
endif()

set(freetype_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/freetype-src/include)
if(WIN32)
  set(freetype_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/freetype-build/Release/freetype.lib)
elseif(UNIX)
  set(freetype_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/freetype-build/Release/libfreetype.a)
endif()

ExternalProject_Add(freetype
  URL https://github.com/freetype/freetype/archive/refs/tags/VER-2-13-2.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/freetype-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/freetype-build
  CMAKE_ARGS
    -DMSDFGEN_CORE_ONLY=ON
    -DMSDFGEN_BUILD_STANDALONE=OFF
    -DMSDFGEN_USE_VCPKG=OFF
    -DCMAKE_OSX_DEPLOYMENT_TARGET=14 #TODO only on MacOs platform
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/freetype-build --config Release
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${freetype_LIBRARIES}
)

message(STATUS "freetype is available in " ${freetype_INCLUDE_DIRS})
message(STATUS "freetype is available in " ${freetype_LIBRARIES})

ExternalProject_Add(msdfgen
  URL https://github.com/Chlumsky/msdfgen/archive/refs/tags/v1.12.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/msdfgen-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/msdfgen-build
  CMAKE_ARGS
    -DMSDFGEN_CORE_ONLY=ON
    -DMSDFGEN_BUILD_STANDALONE=OFF
    -DMSDFGEN_USE_VCPKG=OFF
    -DCMAKE_OSX_DEPLOYMENT_TARGET=14 #TODO only on MacOs platform
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/msdfgen-build --config Release
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${msdfgen_LIBRARIES}
)

message(STATUS "msdfgen is available in " ${msdfgen_INCLUDE_DIRS})
message(STATUS "msdfgen is available in " ${msdfgen_LIBRARIES})

set(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if (BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
else()
  set(LIB_TYPE STATIC)
endif()

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
add_subdirectory("Sources/HodEngine/Input")

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
