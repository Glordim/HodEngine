cmake_minimum_required(VERSION 3.4...3.27)

project(Game LANGUAGES CXX)

set(HodEngine_DIR [[ENGINE_PATH]])
include(${HodEngine_DIR}/CMake/Global.cmake)
include(${HodEngine_DIR}/CMake/FindHodEngine.cmake)
FindHodEngine()

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

if (APPLE)
	add_library(Game SHARED ${Srcs} ${Includes})
	target_include_directories(Game PRIVATE ${PROJECT_SOURCE_DIR})
else()
	add_library(Game SHARED ${Srcs} ${Includes})
	set_property(TARGET Game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodApplication)

endif()

target_include_directories(Game PUBLIC
	${CMAKE_SOURCE_DIR}/Sources
	${HodEngine_INCLUDE_DIRS}
)

target_link_libraries(Game
	${HodEngine_CORE_LIBRARY}
	${HodEngine_WINDOW_LIBRARY}
	${HodEngine_RENDERER_LIBRARY}
	${HodEngine_INPUT_LIBRARY}
	${HodEngine_PHYSICS_LIBRARY}
	${HodEngine_GAME_LIBRARY}
	${HodEngine_IMGUI_LIBRARY}
	${HodEngine_EDITOR_LIBRARY}
)

target_compile_definitions(Game PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(Game PRIVATE PONG_EXPORT)

