cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Physics)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${box2d_INCLUDE_DIRS}
)

add_library(Physics ${LIB_TYPE} ${Srcs} ${Includes})
set_property(TARGET Physics PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Physics Core Renderer box2d) #Remove Renderer dep

target_link_libraries(Physics
	Core
	Renderer
	${box2d_LIBRARIES}
)

target_compile_definitions(Physics PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Physics PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Physics PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Physics PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Physics PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Physics PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Physics PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HOD_EXPORT)
