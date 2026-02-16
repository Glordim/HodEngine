cmake_minimum_required(VERSION 3.10)

project([[PROJECT_NAME]] LANGUAGES CXX)
find_package(HodEngine
	HINTS "[[ROOT_ENGINE_DIR]]/cmake"
	NO_DEFAULT_PATH
)

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

if (APPLE)
	add_library([[PROJECT_NAME]] SHARED ${Srcs} ${Includes})
	target_include_directories([[PROJECT_NAME]] PRIVATE ${PROJECT_SOURCE_DIR})
else()
	add_library([[PROJECT_NAME]] SHARED ${Srcs} ${Includes})

	#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodApplication)
endif()

target_include_directories([[PROJECT_NAME]] PUBLIC
	${PROJECT_SOURCE_DIR}/Sources
)

target_link_libraries([[PROJECT_NAME]]
	HodEngine::Core
	HodEngine::Window
	HodEngine::Renderer
	HodEngine::Input
	HodEngine::Physics
	HodEngine::Game
	HodEngine::ImGui
	HodEngine::Editor
	HodEngine::Audio
	HodEngine::UI
	HodEngine::Application
)

target_compile_definitions([[PROJECT_NAME]]
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE [[PROJECT_EXPORT]]
)

