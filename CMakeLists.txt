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

unset(BUILD_BENCHMARKS CACHE)
option(BUILD_BENCHMARKS "Enable Benchmarks compilation" ON)

unset(BUILD_UNIT_TESTS CACHE)
option(BUILD_UNIT_TESTS "Enable UnitTests compilation" ON)

unset(RENDERER_ENABLE_VALIDATION_LAYER CACHE)
option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" ON)

unset(BUILD_EDITOR CACHE)
option(BUILD_EDITOR "Build Editor modules" ON)

include(Global)
include(FetchContent)
include(ExternalProject)

if(WIN32)
	set(LIB_PREFIX "")
	set(LIB_EXTENSION "lib")
	set(DLL_EXTENSION "dll")
	set(DLL_DIR "bin")
elseif(APPLE)
	set(LIB_PREFIX "lib")
	set(LIB_EXTENSION "a")
	set(DLL_EXTENSION "dylib")
	set(DLL_DIR "lib")
else()
	set(LIB_PREFIX "lib")
	set(LIB_EXTENSION "a")
	set(DLL_EXTENSION "so")
	set(DLL_DIR "lib")
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

include(ThirdParty)

if (BUILD_BENCHMARKS)
	add_subdirectory("Sources/Benchmarks/Core")

	set_target_properties(BenchmarksCore PROPERTIES FOLDER "Benchmark/")
endif()

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

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Sources/HodEngine/HodEngine.hpp"
	DESTINATION "include/HodEngine"
)
configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/CMake/Global.cmake"
		"${CMAKE_BINARY_DIR}/Output/cmake/Global.cmake"
		COPYONLY
)
