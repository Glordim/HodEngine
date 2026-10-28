project(TestsGameSystems)

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

add_executable(TestsGameSystems ${Srcs} ${Includes})
set_property(TARGET TestsGameSystems PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(TestsGameSystems GameSystems)

target_include_directories(TestsGameSystems
	PRIVATE
		"../.."
)

target_link_libraries(TestsGameSystems
	PRIVATE
		GameSystems
		GTest::gtest
		GTest::gtest_main
)

target_compile_definitions(TestsGameSystems PUBLIC ${HOD_PLATFORM_DEFINE})

include(GoogleTest)
gtest_discover_tests(TestsGameSystems)
