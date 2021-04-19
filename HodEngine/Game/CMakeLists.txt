# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

project(Game)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)

assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
".."
"${SDK_DIR}/SDL/include"
"${SDK_DIR}/glm"
"${SDK_DIR}/rapidjson/include"
)

add_library(Game ${Srcs})

add_dependencies(Game Core DebugLayer ImGui Application Physics Renderer)

target_link_libraries(Game
Core
DebugLayer
ImGui
Application
Physics
Renderer
#optimized "../../../Build/HodEngine/Core/Release/Core" debug "../../../Build/HodEngine/Core/Debug/Core"
#optimized "../../../Build/HodEngine/DebugLayer/Release/DebugLayer" debug "../../../Build/HodEngine/DebugLayer/Debug/DebugLayer"
#optimized "../../../Build/HodEngine/ImGui/Release/ImGui" debug "../../../Build/HodEngine/ImGui/Debug/ImGui"
#optimized "../../../Build/HodEngine/Application/Release/Application" debug "../../../Build/HodEngine/Application/Debug/Application"
#optimized "../../../Build/HodEngine/Physics/Release/Physics" debug "../../../Build/HodEngine/Physics/Debug/Physics"
#optimized "../../../Build/HodEngine/Renderer/Release/Renderer" debug "../../../Build/HodEngine/Renderer/Debug/Renderer"
)
