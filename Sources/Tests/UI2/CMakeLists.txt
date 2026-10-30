project(TestsUI2)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE Includes
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.inl"
	"${PROJECT_SOURCE_DIR}/*.h"
)

assign_source_group(${Srcs}) # for VS

add_executable(TestsUI2 ${Srcs} ${Includes})
set_property(TARGET TestsUI2 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(TestsUI2 UI2)

target_include_directories(TestsUI2
	PRIVATE
		"../.."
)

target_link_libraries(TestsUI2
	PRIVATE
		UI2
		GTest::gtest
		GTest::gtest_main
)

target_compile_definitions(TestsUI2 PUBLIC ${HOD_PLATFORM_DEFINE})

include(GoogleTest)
gtest_discover_tests(TestsUI2)
