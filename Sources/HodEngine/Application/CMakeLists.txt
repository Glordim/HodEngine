cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if (APPLE)
    project(Application LANGUAGES CXX OBJCXX)
else()
    project(Application LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Application ${LIB_TYPE} ${Srcs} ${Includes})
set_property(TARGET Application PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Application Core Audio Window Renderer Editor Input Physics)
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
	Editor
	Input
	Physics
)

target_compile_definitions(Application PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(Application PRIVATE HOD_APPLICATION_EXPORT)
add_compile_definitions_for_configs(Application)
