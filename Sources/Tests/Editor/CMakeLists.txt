project(TestsEditor)

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

add_executable(TestsEditor ${Srcs} ${Includes})
set_property(TARGET TestsEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

target_include_directories(TestsEditor
	PRIVATE
		"../.."
)

target_link_libraries(TestsEditor
	PRIVATE
		Editor
		GTest::gtest
		GTest::gtest_main
)

target_compile_definitions(TestsEditor PUBLIC ${HOD_PLATFORM_DEFINE})

include(GoogleTest)
gtest_discover_tests(TestsEditor)
