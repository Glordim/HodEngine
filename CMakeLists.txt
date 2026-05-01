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

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if (BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
else()
	set(LIB_TYPE STATIC)
endif()

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
elseif(ANDROID)
	if(ANDROID_ABI STREQUAL "arm64-v8a")
		set(ARCH_FOLDER "Android_arm64-v8a")
	elseif(ANDROID_ABI STREQUAL "x86_64")
		set(ARCH_FOLDER "Android_x86_64")
	else()
		message(FATAL_ERROR "Unsupported Android ABI: ${ANDROID_ABI}")
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

find_package(fmt REQUIRED CONFIG
	PATHS "${THIRDPARTY_ROOT}/lib/cmake"
	NO_DEFAULT_PATH
)

install(DIRECTORY "${THIRDPARTY_ROOT}/lib/cmake/fmt"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
install(FILES "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}fmt.${LIB_EXTENSION}"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib"
)
install(DIRECTORY "${THIRDPARTY_ROOT}/include/fmt"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
)

# Copy fmt to build tree so HodEngineConfig.cmake works without install
file(COPY "${THIRDPARTY_ROOT}/lib/cmake/fmt"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
file(COPY "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}fmt.${LIB_EXTENSION}"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib"
)
file(COPY "${THIRDPARTY_ROOT}/include/fmt"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
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

add_library(Benchmark::Benchmark STATIC IMPORTED GLOBAL)
set_target_properties(Benchmark::Benchmark PROPERTIES
	IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}benchmark.${LIB_EXTENSION}"
	INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include/benchmark"
	INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/include/benchmark"
	INTERFACE_COMPILE_DEFINITIONS "BENCHMARK_STATIC_DEFINE"
	INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:shlwapi>"
)
add_library(BenchmarkMain::BenchmarkMain STATIC IMPORTED GLOBAL)
set_target_properties(BenchmarkMain::BenchmarkMain PROPERTIES
	IMPORTED_LOCATION "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}benchmark_main.${LIB_EXTENSION}"
	INTERFACE_LINK_LIBRARIES "Benchmark::Benchmark"
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

if(ANDROID)
	# Cross-compilation: use host tools to compile shaders
	if(CMAKE_HOST_WIN32)
		set(_HOST_THIRDPARTY "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Install/Win_x64")
		set(SLANG_COMPILER "${_HOST_THIRDPARTY}/bin/slangc.exe")
		set(SLANG_DLL "${_HOST_THIRDPARTY}/bin/slang.dll")
		set(SLANG_DLL_GLSLANG "${_HOST_THIRDPARTY}/bin/slang-glslang.dll")
	elseif(CMAKE_HOST_APPLE)
		set(_HOST_THIRDPARTY "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Install/MacOs_arm64")
		set(SLANG_COMPILER "${_HOST_THIRDPARTY}/bin/slangc")
		set(SLANG_DLL "${_HOST_THIRDPARTY}/lib/libslang.dylib")
		set(SLANG_DLL_GLSLANG "${_HOST_THIRDPARTY}/lib/libslang-glslang.dylib")
	else()
		set(_HOST_THIRDPARTY "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Install/Linux_x64")
		set(SLANG_COMPILER "${_HOST_THIRDPARTY}/bin/slangc")
		set(SLANG_DLL "${_HOST_THIRDPARTY}/lib/libslang.so")
		set(SLANG_DLL_GLSLANG "${_HOST_THIRDPARTY}/lib/libslang-glslang.so")
	endif()
	unset(_HOST_THIRDPARTY)
elseif(WIN32)
	set(SLANG_COMPILER "${THIRDPARTY_ROOT}/bin/slangc.exe")
	set(SLANG_DLL "${THIRDPARTY_ROOT}/bin/${LIB_PREFIX}slang.${DLL_EXTENSION}")
	set(SLANG_DLL_GLSLANG "${THIRDPARTY_ROOT}/bin/${LIB_PREFIX}slang-glslang.${DLL_EXTENSION}")
else()
	set(SLANG_COMPILER "${THIRDPARTY_ROOT}/bin/slangc")
	set(SLANG_DLL "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}slang.${DLL_EXTENSION}")
	set(SLANG_DLL_GLSLANG "${THIRDPARTY_ROOT}/lib/${LIB_PREFIX}slang-glslang.${DLL_EXTENSION}")
endif()

option(BUILD_BENCHMARKS "Enable Benchmarks compilation" ON)
if (BUILD_BENCHMARKS)
	add_subdirectory("Sources/Benchmarks/Core")

	set_target_properties(BenchmarksCore PROPERTIES FOLDER "Benchmark/")
endif()

option(BUILD_UNIT_TESTS "Enable UnitTests compilation" ON)
if (BUILD_UNIT_TESTS)
	include(CTest)
	enable_testing()
	add_subdirectory("Sources/Tests/Core")

	set_target_properties(TestsCore PROPERTIES FOLDER "Test/")
endif()

set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("Sources/HodEngine/Core")
add_subdirectory("Sources/HodEngine/Math")
add_subdirectory("Sources/HodEngine/GameSystems")
add_subdirectory("Sources/HodEngine/Window")
add_subdirectory("Sources/HodEngine/Renderer")
add_subdirectory("Sources/HodEngine/ImGui")
add_subdirectory("Sources/HodEngine/Physics")
add_subdirectory("Sources/HodEngine/Game")
add_subdirectory("Sources/HodEngine/UI")
add_subdirectory("Sources/HodEngine/Input")
add_subdirectory("Sources/HodEngine/Audio")
add_subdirectory("Sources/HodEngine/Application")
add_subdirectory("Sources/HodEngine/Main")

option(BUILD_EDITOR "Build Editor modules" ON)
if (BUILD_EDITOR AND NOT ANDROID)
	add_subdirectory("Sources/HodEngine/CoreEditor")
	add_subdirectory("Sources/HodEngine/InputEditor")
	add_subdirectory("Sources/HodEngine/GameEditor")
	add_subdirectory("Sources/HodEngine/UIEditor")
	add_subdirectory("Sources/HodEngine/Editor")
	add_subdirectory("Sources/HodEditor")
endif()

install(EXPORT HodEngineTargets
	NAMESPACE HodEngine::
	FILE HodEngineTargets.cmake
	DESTINATION "${HOD_PLATFORM_SUBDIR}/${HOD_LIB_TYPE_SUBDIR}/cmake"
)

configure_file(cmake/HodEngineConfig.cmake.in
	"${CMAKE_BINARY_DIR}/Output/cmake/HodEngineConfig.cmake" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/Output/cmake/HodEngineConfig.cmake"
	DESTINATION "cmake")

export(EXPORT HodEngineTargets
	FILE "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/${HOD_LIB_TYPE_SUBDIR}/cmake/HodEngineTargets.cmake"
	NAMESPACE HodEngine::
)

if(CMAKE_TOOLCHAIN_FILE)
	install(FILES "${CMAKE_TOOLCHAIN_FILE}"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/${HOD_LIB_TYPE_SUBDIR}/cmake"
		RENAME "toolchain.cmake")

	configure_file(
		"${CMAKE_TOOLCHAIN_FILE}"
		"${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/${HOD_LIB_TYPE_SUBDIR}/cmake/toolchain.cmake"
		COPYONLY
	)
endif()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Global.cmake"
		DESTINATION "cmake")
configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/CMake/Global.cmake"
		"${CMAKE_BINARY_DIR}/Output/cmake/Global.cmake"
		COPYONLY
)
