cmake_minimum_required(VERSION 3.10)

project([[PROJECT_NAME]] LANGUAGES CXX)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

set(HodEngine_DIR [[ENGINE_PATH]])
include(${HodEngine_DIR}/CMake/Global.cmake)
SetupLanguageVersion()
SetupPlatform()
SetupCompilation()

include(${HodEngine_DIR}/CMake/FindHodEngine.cmake)
FindHodEngine()

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/Sources/*.cpp"
	"${PROJECT_SOURCE_DIR}/Sources/*.hpp"
	"${PROJECT_SOURCE_DIR}/Sources/*.inl"
	"${PROJECT_SOURCE_DIR}/Sources/*.h"
)

file(GLOB_RECURSE Includes
	"${PROJECT_SOURCE_DIR}/Sources/*.hpp"
	"${PROJECT_SOURCE_DIR}/Sources/*.inl"
	"${PROJECT_SOURCE_DIR}/Sources/*.h"
)

assign_source_group(${Srcs}) # for VS

if (APPLE)
	add_library([[PROJECT_NAME]] SHARED ${Srcs} ${Includes})
	target_include_directories([[PROJECT_NAME]] PRIVATE ${PROJECT_SOURCE_DIR})
else()
	add_library([[PROJECT_NAME]] SHARED ${Srcs} ${Includes})
	set_property(TARGET [[PROJECT_NAME]] PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

	#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodApplication)
endif()

target_include_directories([[PROJECT_NAME]] PUBLIC
	${PROJECT_SOURCE_DIR}/Sources
	${HodEngine_INCLUDE_DIRS}
)

target_link_libraries([[PROJECT_NAME]]
	${HodEngine_CORE_LIBRARY}
	${HodEngine_WINDOW_LIBRARY}
	${HodEngine_RENDERER_LIBRARY}
	${HodEngine_INPUT_LIBRARY}
	${HodEngine_PHYSICS_LIBRARY}
	${HodEngine_GAME_LIBRARY}
	${HodEngine_IMGUI_LIBRARY}
	${HodEngine_EDITOR_LIBRARY}
	${HodEngine_AUDIO_LIBRARY}
	${HodEngine_UI_LIBRARY}
	${HodEngine_APPLICATION_LIBRARY}
	${HodEngine_HOD_APPLICATION_LIBRARY}
)

target_compile_definitions([[PROJECT_NAME]] PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions([[PROJECT_NAME]] PRIVATE [[PROJECT_EXPORT]])

