cmake_minimum_required(VERSION 3.10)

project(BenchmarksCore)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

assign_source_group(${Srcs}) # for VS

add_executable(BenchmarksCore ${Srcs})
set_property(TARGET BenchmarksCore PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(BenchmarksCore Core)

target_include_directories(BenchmarksCore
	PRIVATE
		"../.."
)

target_link_libraries(BenchmarksCore
	PRIVATE
		Core
		benchmark::benchmark
		benchmark::benchmark_main
)

target_compile_definitions(BenchmarksCore PUBLIC ${HOD_PLATFORM_DEFINE})
