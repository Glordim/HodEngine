project(TestsText)

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

add_executable(TestsText ${Srcs} ${Includes})
set_property(TARGET TestsText PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(TestsText Text)

target_include_directories(TestsText
	PRIVATE
		"../.."
)

target_link_libraries(TestsText
	PRIVATE
		Text
		GTest::gtest
		GTest::gtest_main
)

target_compile_definitions(TestsText
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE TESTS_TEXT_FIXTURES_DIR="${PROJECT_SOURCE_DIR}/Fixtures"
)

include(GoogleTest)
gtest_discover_tests(TestsText)
