cmake_minimum_required(VERSION 3.4...3.27)

project(HodEngine)

if (NOT TARGET Core)
	add_subdirectory(Core)
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

add_library(HodEngine STATIC HodEngine.cpp)

target_link_libraries(HodEngine
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

set_target_properties(Core PROPERTIES FOLDER "HodEngine")
set_target_properties(Input PROPERTIES FOLDER "HodEngine")
set_target_properties(Application PROPERTIES FOLDER "HodEngine")
set_target_properties(ImGui PROPERTIES FOLDER "HodEngine")
set_target_properties(Physics PROPERTIES FOLDER "HodEngine")
set_target_properties(Window PROPERTIES FOLDER "HodEngine")
set_target_properties(Renderer PROPERTIES FOLDER "HodEngine")
set_target_properties(Game PROPERTIES FOLDER "HodEngine")
set_target_properties(Editor PROPERTIES FOLDER "HodEngine")
