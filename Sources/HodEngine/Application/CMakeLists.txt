cmake_minimum_required(VERSION 3.10)

include(Global)

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
target_precompile_headers(Application PRIVATE "Pch.hpp")
set_property(TARGET Application PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Application Core Audio Window Renderer Input Physics Game ImGui UI)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)
get_target_property(Renderer_INCLUDES Renderer INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${Core_INCLUDES}
	${Window_INCLUDES}
	${Renderer_INCLUDES}
)

target_link_libraries(Application
	Core
	Audio
	Window
	Renderer
	Input
	Physics
	Game
	ImGui
	UI
)

if (ANDROID)
find_library(android-lib android)
target_link_libraries(Application ${android-lib} log)
target_include_directories(Application PRIVATE ${CMAKE_SOURCE_DIR}/Externals/agdk-libraries-2023.3.0.0/include)
target_link_libraries(Application ${CMAKE_SOURCE_DIR}/Externals/agdk-libraries-2023.3.0.0/libs/${ANDROID_ABI}_cpp_static_Release/libgame-activity_static.a)
endif()

target_compile_definitions(Application PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Application PRIVATE HOD_APPLICATION_EXPORT)
