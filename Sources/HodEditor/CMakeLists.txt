cmake_minimum_required(VERSION 3.10)

include(Global)

if (APPLE)
	project(HodEditor LANGUAGES CXX OBJCXX)
else()
	project(HodEditor LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_executable(HodEditor ${Srcs} ${Includes})
set_target_properties(HodEditor PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
	VS_STARTUP_PROJECT "HodEditor"
)

target_include_directories(HodEditor
	PRIVATE "../.."
)
#target_precompile_headers(HodEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

add_dependencies(HodEditor
	Application
	Editor
	CoreEditor
	InputEditor
	GameEditor
	UIEditor
)

target_link_libraries(HodEditor
	PUBLIC Application Editor
)

target_compile_definitions(HodEditor PUBLIC ${PLATFORM_DEFINE})

install(TARGETS HodEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

