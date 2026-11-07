project(TestsVersionControl)

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

add_executable(TestsVersionControl ${Srcs} ${Includes})
set_property(TARGET TestsVersionControl PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(TestsVersionControl VersionControlEditor)

target_include_directories(TestsVersionControl
	PRIVATE
		"../.."
)

target_link_libraries(TestsVersionControl
	PRIVATE
		VersionControlEditor
		GTest::gtest
		GTest::gtest_main
)

target_compile_definitions(TestsVersionControl PUBLIC ${HOD_PLATFORM_DEFINE})

include(GoogleTest)
gtest_discover_tests(TestsVersionControl)
