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

if(WIN32)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        set(ARCH_FOLDER "Win_${CMAKE_GENERATOR_PLATFORM}")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
        set(ARCH_FOLDER "Win_${CMAKE_GENERATOR_PLATFORM}")
    else()
        message(FATAL_ERROR "Unsupported Windows architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
elseif(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(ARCH_FOLDER "MacOs_arm64")
    else()
        message(FATAL_ERROR "Unsupported Apple architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
else()
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

set(THIRDPARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Install/${ARCH_FOLDER}")

add_library(Box2d::Box2d STATIC IMPORTED GLOBAL)
set_target_properties(Box2d::Box2d PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}box2d.${LIB_EXTENSION}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(Freetype::Freetype STATIC IMPORTED GLOBAL)
set_target_properties(Freetype::Freetype PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}freetype.${LIB_EXTENSION}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include/freetype2"
)

add_library(SpirvCross::SpirvCross STATIC IMPORTED GLOBAL)
set_target_properties(SpirvCross::SpirvCross PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}spirv-cross-core.${LIB_EXTENSION}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(Stb::Stb STATIC IMPORTED GLOBAL)
set_target_properties(Stb::Stb PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(VulkanMemoryAllocator::VulkanMemoryAllocator STATIC IMPORTED GLOBAL)
set_target_properties(VulkanMemoryAllocator::VulkanMemoryAllocator PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(PortableFileDialogs::PortableFileDialogs STATIC IMPORTED GLOBAL)
set_target_properties(PortableFileDialogs::PortableFileDialogs PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

if(WIN32)
set(SLANG_COMPILER "${THIRDPARTY_ROOT}/bin/slangc.exe")
set(SLANG_DLL "${THIRDPARTY_ROOT}/bin/slang.dll")
set(SLANG_DLL_GLSLANG "${THIRDPARTY_ROOT}/bin/slang-glslang.dll")
else()
set(SLANG_COMPILER "${THIRDPARTY_ROOT}/bin/slangc")
endif()

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
add_subdirectory("Sources/HodEngine/UI")
add_subdirectory("Sources/HodEngine/Input")
add_subdirectory("Sources/HodEngine/Audio")
add_subdirectory("Sources/HodEngine/Application")

add_subdirectory("Sources/HodApplication")

if (NOT ANDROID)
  add_subdirectory("Sources/HodEngine/CoreEditor")
  add_subdirectory("Sources/HodEngine/GameEditor")
  add_subdirectory("Sources/HodEngine/UIEditor")
  add_subdirectory("Sources/HodEngine/Editor")
  add_subdirectory("Sources/HodEditor")
endif()
