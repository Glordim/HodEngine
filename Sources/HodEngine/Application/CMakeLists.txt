cmake_minimum_required(VERSION 3.10)

include(Global)
include(ClangBuildAnalyzer)

if (APPLE)
	project(Application LANGUAGES CXX OBJCXX)
else()
	project(Application LANGUAGES CXX)
endif()

if (APPLE)
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES Win32 Android Linux)
elseif(ANDROID)
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES MacOs Win32 Linux)
elseif(WIN32)
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES MacOs Android Linux)
else() # Linux
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES Win32 Android MacOs)
endif()

assign_source_group(${Srcs}) # for VS

add_library(Application ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Application ALIAS Application)
set_target_properties(Application PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(Application PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Application PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Application
	PUBLIC Core Audio Window Renderer Input Physics Game ImGui UI
)

if (ANDROID)
	find_library(android-lib android)
	target_link_libraries(Application ${android-lib} log)
	target_include_directories(Application PRIVATE ${CMAKE_SOURCE_DIR}/Externals/agdk-libraries-2023.3.0.0/include)
	target_link_libraries(Application ${CMAKE_SOURCE_DIR}/Externals/agdk-libraries-2023.3.0.0/libs/${ANDROID_ABI}_cpp_static_Release/libgame-activity_static.a)
endif()

target_compile_definitions(Application PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Application PRIVATE HOD_APPLICATION_EXPORT)

install(TARGETS Application
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(Application)
