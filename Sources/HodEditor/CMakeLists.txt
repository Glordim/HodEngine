cmake_minimum_required(VERSION 3.10)

include(Global)

if (APPLE)
    project(HodEditor LANGUAGES CXX OBJCXX)
else()
    project(HodEditor LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

if (APPLE)
	add_executable(HodEditor ${Srcs} ${Includes})
	target_include_directories(HodEditor PRIVATE ${PROJECT_SOURCE_DIR})
else()
	add_executable(HodEditor ${Srcs} ${Includes})
	set_property(TARGET HodEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodEditor)

endif()

include_directories(
	".."
)

add_dependencies(HodEditor
	Application
	Editor
	CoreEditor
	GameEditor
	UIEditor
)

target_link_libraries(HodEditor
	Application
	Editor
)

target_compile_definitions(HodEditor PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})

