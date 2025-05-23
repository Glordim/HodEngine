cmake_minimum_required(VERSION 3.10)

project(Hod) # Rename

option(ENABLE_UNITY_BUILD "Enable Unity Build for faster compilation" ON)
if (ENABLE_UNITY_BUILD)
    set(CMAKE_UNITY_BUILD ON)
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(Global)
include(FetchContent)
include(ExternalProject)

SetupLanguageVersion()

if(WIN32)
  set(LIB_PREFIX "")
  set(LIB_EXTENSION "lib")
else()
  set(LIB_PREFIX "lib")
  set(LIB_EXTENSION "a")
endif()

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
  set(LIB_SUB_FOLDER "Release")
else()
  set(LIB_SUB_FOLDER ".")
endif()

FetchContent_Declare(
  stb
  URL https://github.com/nothings/stb/archive/refs/heads/master.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(stb)
message(STATUS "stb is available in " ${stb_SOURCE_DIR})

FetchContent_Declare(
  portable_file_dialogs
  URL https://github.com/samhocevar/portable-file-dialogs/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(portable_file_dialogs)
message(STATUS "portable_file_dialogs is available in " ${portable_file_dialogs_SOURCE_DIR})

set(box2d_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/box2d-src/include)
set(box2d_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/box2d-build/src/${LIB_SUB_FOLDER}/${LIB_PREFIX}box2d.${LIB_EXTENSION})

if (WIN32)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

ExternalProject_Add(box2d
  URL https://github.com/erincatto/box2d/archive/28adacf82377d4113f2ed00586141463244b9d10.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/box2d-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/box2d-build
  CMAKE_ARGS
    -DBOX2D_SAMPLES=OFF
    -DBOX2D_VALIDATE=OFF
    -DBOX2D_UNIT_TESTS=OFF
    -DBOX2D_AVX2=OFF # TODO crash on Android
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_OSX_DEPLOYMENT_TARGET=14 #TODO only on MacOs platform
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_BUILD_TYPE=Release
    -G ${CMAKE_GENERATOR}
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/box2d-build --config Release --parallel
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${box2d_LIBRARIES}
)

message(STATUS "box2d is available in " ${box2d_INCLUDE_DIRS})
message(STATUS "box2d is available in " ${box2d_LIBRARIES})

set(freetype_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/freetype-src/include)
set(freetype_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/freetype-build/${LIB_SUB_FOLDER}/${LIB_PREFIX}freetype.${LIB_EXTENSION})

ExternalProject_Add(freetype
  URL https://github.com/freetype/freetype/archive/refs/tags/VER-2-13-2.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/freetype-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/freetype-build
  CMAKE_ARGS
    -DFT_DISABLE_BROTLI=ON
    -DFT_DISABLE_HARFBUZZ=ON
    -DFT_DISABLE_PNG=ON
    -DFT_DISABLE_BZIP2=ON
    -DFT_DISABLE_ZLIB=ON
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_OSX_DEPLOYMENT_TARGET=14 #TODO only on MacOs platform
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_BUILD_TYPE=Release
    -G ${CMAKE_GENERATOR}
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/freetype-build --config Release --parallel
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${freetype_LIBRARIES}
)

message(STATUS "freetype is available in " ${freetype_INCLUDE_DIRS})
message(STATUS "freetype is available in " ${freetype_LIBRARIES})

set(msdfgen_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/msdfgen-src/include)
set(msdfgen_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/msdfgen-build/${LIB_SUB_FOLDER}/${LIB_PREFIX}msdfgen-core.${LIB_EXTENSION})

ExternalProject_Add(msdfgen
  URL https://github.com/Chlumsky/msdfgen/archive/refs/tags/v1.12.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/msdfgen-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/msdfgen-build
  CMAKE_ARGS
    -DMSDFGEN_CORE_ONLY=ON
    -DMSDFGEN_BUILD_STANDALONE=OFF
    -DMSDFGEN_USE_VCPKG=OFF
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_OSX_DEPLOYMENT_TARGET=14 #TODO only on MacOs platform
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_BUILD_TYPE=Release
    -G ${CMAKE_GENERATOR}
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/msdfgen-build --config Release --parallel
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${msdfgen_LIBRARIES}
)

message(STATUS "msdfgen is available in " ${msdfgen_INCLUDE_DIRS})
message(STATUS "msdfgen is available in " ${msdfgen_LIBRARIES})

# TODO move in Renderer CmakeList (only with Vulkan) ? 

set(glslang_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/glslang-src)
set(glslang_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/glslang-build/glslang/${LIB_SUB_FOLDER}/${LIB_PREFIX}glslang.${LIB_EXTENSION} ${CMAKE_BINARY_DIR}/_deps/glslang-build/glslang/${LIB_SUB_FOLDER}/${LIB_PREFIX}glslang-default-resource-limits.${LIB_EXTENSION})

ExternalProject_Add(glslang
  URL https://github.com/KhronosGroup/glslang/archive/refs/tags/15.0.0.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/glslang-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/glslang-build
  CMAKE_ARGS
    -DGLSLANG_TESTS=OFF
    -DBUILD_EXTERNAL=OFF
    -DENABLE_SPIRV=ON
    -DENABLE_HLSL=OFF
    -DENABLE_SPVREMAPPER=OFF
    -DENABLE_GLSLANG_BINARIES=OFF
    -DENABLE_OPT=OFF
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_BUILD_TYPE=Release
    -G ${CMAKE_GENERATOR}
  BUILD_COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/glslang-build --config Release --parallel
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${glslang_LIBRARIES}
)

message(STATUS "glslang is available in " ${glslang_INCLUDE_DIRS})
message(STATUS "glslang is available in " ${glslang_LIBRARIES})

ExternalProject_Add(VulkanMemoryAllocator
  URL https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.1.0.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/VulkanMemoryAllocator-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/VulkanMemoryAllocator-build
  CMAKE_ARGS
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_BUILD_TYPE=Release
    -G ${CMAKE_GENERATOR}
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${VulkanMemoryAllocator_LIBRARIES}
)

set(VulkanMemoryAllocator_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/VulkanMemoryAllocator-src/include)

message(STATUS "VulkanMemoryAllocator is available in " ${VulkanMemoryAllocator_INCLUDE_DIRS})
message(STATUS "VulkanMemoryAllocator is available in " ${VulkanMemoryAllocator_LIBRARIES})

set(SpirvCross_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/_deps/SpirvCross-src)
set(SpirvCross_LIBRARIES ${CMAKE_BINARY_DIR}/_deps/SpirvCross-build/${LIB_SUB_FOLDER}/${LIB_PREFIX}spirv-cross-core.${LIB_EXTENSION})

ExternalProject_Add(SpirvCross
  URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/vulkan-sdk-1.3.275.0.zip
  DOWNLOAD_DIR ${DOWNLOAD_CACHE_DIR}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/SpirvCross-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/SpirvCross-build
  CMAKE_ARGS
    -DSPIRV_CROSS_CLI=OFF
    -DSPIRV_CROSS_ENABLE_TESTS=OFF
    -DSPIRV_CROSS_ENABLE_HLSL=OFF
    -DSPIRV_CROSS_ENABLE_MSL=OFF
    -DSPIRV_CROSS_ENABLE_CPP=OFF
    -DSPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS=ON
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_UNITY_BUILD=ON
    -G ${CMAKE_GENERATOR}
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/_deps/SpirvCross-build --config Release --parallel
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${SpirvCross_LIBRARIES}
)

message(STATUS "SpirvCross is available in " ${SpirvCross_INCLUDE_DIRS})
message(STATUS "SpirvCross is available in " ${SpirvCross_LIBRARIES})

SetupPlatform()
SetupCompilation()

option(BUILD_UNIT_TESTS "Enable UnitTests compilation" ON)
if (BUILD_UNIT_TESTS)
  include(CTest)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  #For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_subdirectory("Sources/Tests/Core")
endif()

set(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK")

set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if (BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
else()
  set(LIB_TYPE STATIC)
endif()

add_subdirectory("Sources/HodEngine/Core")
add_subdirectory("Sources/HodEngine/Window")
add_subdirectory("Sources/HodEngine/Renderer")
add_subdirectory("Sources/HodEngine/ImGui")
add_subdirectory("Sources/HodEngine/Physics")
add_subdirectory("Sources/HodEngine/Game")
add_subdirectory("Sources/HodEngine/Input")
add_subdirectory("Sources/HodEngine/Audio")
add_subdirectory("Sources/HodEngine/Application")

add_subdirectory("Sources/HodApplication")

if (NOT ANDROID)
  add_subdirectory("Sources/HodEngine/Editor")
  add_subdirectory("Sources/HodEditor")
endif()
