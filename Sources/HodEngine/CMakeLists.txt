cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(HodEngine)

if (NOT TARGET Core)
	add_subdirectory(Core)
endif()

if (NOT TARGET Audio)
	add_subdirectory(Audio)
endif()

if (NOT TARGET Input)
	add_subdirectory(Input)
endif()

if (NOT TARGET Physics)
	add_subdirectory(Physics)
endif()

if (NOT TARGET Window)
	add_subdirectory(Window)
endif()

if (NOT TARGET Renderer)
	add_subdirectory(Renderer)
endif()

if (NOT TARGET Game)
	add_subdirectory(Game)
endif()

if (NOT TARGET ImGui)
	add_subdirectory(ImGui)
endif()

if (NOT TARGET Application)
	add_subdirectory(Application)
endif()

if (NOT TARGET Editor)
	add_subdirectory(Editor)
endif()

assign_source_group("")

add_library(HodEngine SHARED HodEngine.cpp)
set_property(TARGET HodEngine PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_force_load_libs(HodEngine
	Core
	Application
	Input
	ImGui
	Physics
	Window
	Renderer
	Game
	Editor
)

target_compile_definitions(HodEngine PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(HodEngine PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HOD_EXPORT)
target_compile_definitions(HodEngine PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HOD_EXPORT)
target_compile_definitions(HodEngine PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HOD_EXPORT)
target_compile_definitions(HodEngine PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HOD_EXPORT)
target_compile_definitions(HodEngine PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HOD_EXPORT)
target_compile_definitions(HodEngine PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HOD_EXPORT)

set_target_properties(Core PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Audio PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Input PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Application PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(ImGui PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Physics PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Window PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Renderer PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Game PROPERTIES FOLDER "HodEngine-Libs")
set_target_properties(Editor PROPERTIES FOLDER "HodEngine-Libs")
