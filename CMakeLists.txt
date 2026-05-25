cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0177 NEW)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

project(Hod) # Rename
include(PlatformDetection)

option(ENABLE_UNITY_BUILD "Enable Unity Build for faster compilation" OFF)
if (ENABLE_UNITY_BUILD)
	set(CMAKE_UNITY_BUILD ON)
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if (BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
else()
	set(LIB_TYPE STATIC)
endif()

option(BUILD_BENCHMARKS "Enable Benchmarks compilation" ON)
option(BUILD_UNIT_TESTS "Enable UnitTests compilation" ON)
option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" ON)
option(BUILD_EDITOR "Build Editor modules" ON)

include(Global)
include(FetchContent)
include(ExternalProject)
include(ThirdParty)

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
add_subdirectory("Sources/HodEngine/UI2")
add_subdirectory("Sources/HodEngine/Input")
add_subdirectory("Sources/HodEngine/Audio")
add_subdirectory("Sources/HodEngine/Application")
add_subdirectory("Sources/HodEngine/Main")

if (BUILD_EDITOR AND NOT ANDROID AND BUILD_SHARED_LIBS)
	add_subdirectory("Sources/HodEngine/CoreEditor")
	add_subdirectory("Sources/HodEngine/InputEditor")
	add_subdirectory("Sources/HodEngine/GameEditor")
	add_subdirectory("Sources/HodEngine/UIEditor")
	add_subdirectory("Sources/HodEngine/UI2Editor")
	add_subdirectory("Sources/HodEngine/Editor")
	add_subdirectory("Sources/HodEditor")
endif()

if (BUILD_BENCHMARKS AND NOT ANDROID)
	add_subdirectory("Sources/Benchmarks/Core")

	set_target_properties(BenchmarksCore PROPERTIES FOLDER "Benchmark/")
endif()

if (BUILD_UNIT_TESTS AND NOT ANDROID)
	include(CTest)
	enable_testing()
	add_subdirectory("Sources/Tests/Core")

	set_target_properties(TestsCore PROPERTIES FOLDER "Test/")
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

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Global.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/CMake/PlatformDetection.cmake"
		DESTINATION "cmake")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Sources/HodEngine/HodEngine.hpp"
	DESTINATION "include/HodEngine"
)
configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/CMake/Global.cmake"
		"${CMAKE_BINARY_DIR}/Output/cmake/Global.cmake"
		COPYONLY
)
configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/CMake/PlatformDetection.cmake"
		"${CMAKE_BINARY_DIR}/Output/cmake/PlatformDetection.cmake"
		COPYONLY
)
