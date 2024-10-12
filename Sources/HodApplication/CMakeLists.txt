cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if (APPLE)
    project(HodApplication LANGUAGES CXX OBJCXX)
else()
    project(HodApplication LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

if (APPLE)
	add_executable(HodApplication ${Srcs} ${Includes})
	target_include_directories(HodApplication PRIVATE ${PROJECT_SOURCE_DIR})
elseif(ANDROID)
	add_library(HodApplication ${Srcs} ${Includes})
	find_library(android-lib android)
	target_link_libraries(HodApplication ${android-lib} log)
	target_include_directories(HodApplication PRIVATE ${CMAKE_SOURCE_DIR}/Externals/agdk-libraries-2023.3.0.0/include)
	target_link_libraries(HodApplication ${CMAKE_SOURCE_DIR}/Externals/agdk-libraries-2023.3.0.0/libs/${ANDROID_ABI}_cpp_static_Release/libgame-activity_static.a)
	target_link_libraries(HodApplication ${CMAKE_SOURCE_DIR}/Externals/agdk-libraries-2023.3.0.0/libs/${ANDROID_ABI}_cpp_static_Release/libgame-text-input_static.a)
else()
	add_executable(HodApplication ${Srcs} ${Includes})
		
	set_property(TARGET HodApplication PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodApplication)

endif()

include_directories(
	".."
)

add_dependencies(HodApplication
	Application
)

target_link_libraries(HodApplication
	Application
)

target_compile_definitions(HodApplication PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
