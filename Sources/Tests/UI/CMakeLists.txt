project(TestsUI)

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

add_executable(TestsUI ${Srcs} ${Includes})
set_property(TARGET TestsUI PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(TestsUI UI)

target_include_directories(TestsUI
	PRIVATE
		"../.."
)

target_link_libraries(TestsUI
	PRIVATE
		UI
		GTest::gtest
		GTest::gtest_main
)

target_compile_definitions(TestsUI PUBLIC ${HOD_PLATFORM_DEFINE})

include(GoogleTest)
gtest_discover_tests(TestsUI)
