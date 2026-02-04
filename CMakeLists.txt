cmake_minimum_required(VERSION 3.10)

project(Hod) # Rename

option(ENABLE_UNITY_BUILD "Enable Unity Build for faster compilation" OFF)
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

if(WIN32)
  set(LIB_PREFIX "")
  set(LIB_EXTENSION "lib")
  set(DLL_EXTENSION "dll")
elseif(APPLE)
  set(LIB_PREFIX "lib")
  set(LIB_EXTENSION "a")
  set(DLL_EXTENSION "dylib")
else()
  set(LIB_PREFIX "lib")
  set(LIB_EXTENSION "a")
  set(DLL_EXTENSION "so")
endif()

if(WIN32)
    if(ARCH STREQUAL "x64")
        set(ARCH_FOLDER "Win_${ARCH}")
    elseif(ARCH STREQUAL "arm64")
        set(ARCH_FOLDER "Win_${ARCH}")
    else()
        message(FATAL_ERROR "Unsupported Windows architecture: ${ARCH}")
    endif()
elseif(APPLE)
    if(ARCH STREQUAL "arm64")
        set(ARCH_FOLDER "MacOs_${ARCH}")
    else()
        message(FATAL_ERROR "Unsupported Apple architecture: ${ARCH}")
    endif()
elseif(UNIX)
    if(ARCH STREQUAL "x64")
        set(ARCH_FOLDER "Linux_${ARCH}")
    else()
        message(FATAL_ERROR "Unsupported Linux architecture: ${ARCH}")
    endif()
else()
    message(FATAL_ERROR "Unsupported system: ${ARCH}")
endif()

set(THIRDPARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Install/${ARCH_FOLDER}")

add_library(Box2d::Box2d STATIC IMPORTED GLOBAL)
set_target_properties(Box2d::Box2d PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}box2d.${LIB_EXTENSION}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(Freetype::Freetype STATIC IMPORTED GLOBAL)
set_target_properties(Freetype::Freetype PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}freetype.${LIB_EXTENSION}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include/freetype2"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include/freetype2"
)

add_library(Fmt::Fmt STATIC IMPORTED GLOBAL)
set_target_properties(Fmt::Fmt PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}fmt.${LIB_EXTENSION}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(Stb::Stb INTERFACE IMPORTED GLOBAL)
set_target_properties(Stb::Stb PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(WindowsHModular::WindowsHModular INTERFACE IMPORTED GLOBAL)
set_target_properties(WindowsHModular::WindowsHModular PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(VulkanMemoryAllocator::VulkanMemoryAllocator INTERFACE IMPORTED GLOBAL)
set_target_properties(VulkanMemoryAllocator::VulkanMemoryAllocator PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(PortableFileDialogs::PortableFileDialogs INTERFACE IMPORTED GLOBAL)
set_target_properties(PortableFileDialogs::PortableFileDialogs PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include"
)

add_library(Gtest::Gtest STATIC IMPORTED GLOBAL)
set_target_properties(Gtest::Gtest PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}gtest.${LIB_EXTENSION}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include/Gtest"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include/Gtest"
)
add_library(GtestMain::GtestMain STATIC IMPORTED GLOBAL)
set_target_properties(GtestMain::GtestMain PROPERTIES
    IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}gtest_main.${LIB_EXTENSION}"
)

if(WIN32)
set(SLANG_COMPILER "${THIRDPARTY_ROOT}/bin/slangc.exe")
set(SLANG_DLL "${THIRDPARTY_ROOT}/bin/${LIB_PREFIX}slang.${DLL_EXTENSION}")
set(SLANG_DLL_GLSLANG "${THIRDPARTY_ROOT}/bin/${LIB_PREFIX}slang-glslang.${DLL_EXTENSION}")
else()
set(SLANG_COMPILER "${THIRDPARTY_ROOT}/bin/slangc")
set(SLANG_DLL "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}slang.${DLL_EXTENSION}")
set(SLANG_DLL_GLSLANG "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}slang-glslang.${DLL_EXTENSION}")
endif()

SetupPlatform()
SetupCompilation()

option(BUILD_UNIT_TESTS "Enable UnitTests compilation" ON)
if (BUILD_UNIT_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory("Sources/Tests/Core")

  set_target_properties(TestsCore PROPERTIES FOLDER "Test/")
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
  add_subdirectory("Sources/HodEngine/InputEditor")
  add_subdirectory("Sources/HodEngine/GameEditor")
  add_subdirectory("Sources/HodEngine/UIEditor")
  add_subdirectory("Sources/HodEngine/Editor")
  add_subdirectory("Sources/HodEditor")
endif()
