cmake_minimum_required(VERSION 3.10)

if(HOD_GAME_BUILD_TYPE STREQUAL "GameModule" OR ANDROID)
	set(BUILD_SHARED_LIBS ON)
	set(HOD_LIB_TYPE "shared")
else()
	set(BUILD_SHARED_LIBS OFF)
	set(HOD_LIB_TYPE "static")
endif()

project([[PROJECT_NAME]] LANGUAGES CXX)
find_package(HodEngine REQUIRED)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

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

if(HOD_GAME_BUILD_TYPE STREQUAL "GameModule" OR ANDROID)
	add_library([[PROJECT_NAME]] SHARED ${Srcs} ${Includes})
else()
	add_executable([[PROJECT_NAME]] ${Srcs} ${Includes})
endif()

install(TARGETS [[PROJECT_NAME]]
	RUNTIME DESTINATION .
	LIBRARY DESTINATION .
	ARCHIVE DESTINATION .
)

target_include_directories([[PROJECT_NAME]] PUBLIC
	${PROJECT_SOURCE_DIR}/Sources
)

target_link_libraries([[PROJECT_NAME]]
	HodEngine::Core
	HodEngine::Math
	HodEngine::Window
	HodEngine::Renderer
	HodEngine::Input
	HodEngine::Physics
	HodEngine::GameSystems
	HodEngine::Game
	HodEngine::ImGui
	HodEngine::Audio
	HodEngine::UI
	HodEngine::Application
)

if(NOT HOD_GAME_BUILD_TYPE STREQUAL "Application")
	target_link_libraries([[PROJECT_NAME]]
		HodEngine::Editor
	)
endif()

target_compile_definitions([[PROJECT_NAME]]
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE [[PROJECT_EXPORT]]
)
