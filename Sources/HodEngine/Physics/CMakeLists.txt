cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Physics)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${box2d_INCLUDE_DIRS}
)

add_library(Physics OBJECT ${Srcs} ${Includes})
set_property(TARGET Physics PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Physics Core Renderer box2d)

target_link_libraries(Physics
	Core
	Renderer
	${box2d_LIBRARIES}
)
