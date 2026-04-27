cmake_minimum_required(VERSION 3.10)

project(GameSystems LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(GameSystems ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::GameSystems ALIAS GameSystems)

set_target_properties(GameSystems PROPERTIES
	FOLDER "HodEngine"
)
target_include_directories(GameSystems PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(GameSystems PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(GameSystems
	PUBLIC HodEngine::Core
)

target_compile_definitions(GameSystems
	PUBLIC ${HOD_PLATFORM_DEFINE}
)

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(GameSystems PUBLIC HOD_GAME_SYSTEMS_STATIC)
else()
	target_compile_definitions(GameSystems PRIVATE HOD_GAME_SYSTEMS_EXPORT)
endif()

hod_install_lib(GameSystems)
