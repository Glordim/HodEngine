cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Editor)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES FileTemplate)

file(GLOB_RECURSE Icons
	"${PROJECT_SOURCE_DIR}/Icons/*.png"
)

foreach(file ${Icons})
  EmbedBinaryInSource(${file}
    ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
    ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
    ${PROJECT_BINARY_DIR}/Icons
    GeneratedFileH
    GeneratedFileCPP
  )
  list(APPEND Srcs ${GeneratedFileH})
  list(APPEND Includes ${GeneratedFileH})
  list(APPEND Srcs ${GeneratedFileCPP})
endforeach()

file(GLOB_RECURSE FileTemplates
	"${PROJECT_SOURCE_DIR}/FileTemplate/*"
)

foreach(file ${FileTemplates})
  EmbedTextInSource(${file}
    ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/TextSourceFileTemplate.hpp.in
    ${PROJECT_BINARY_DIR}/FileTemplates
    GeneratedFileH
  )
  list(APPEND Srcs ${GeneratedFileH})
  list(APPEND Includes ${GeneratedFileH})
endforeach()

assign_source_group(${Srcs}) # for VS

find_package(Stb REQUIRED)
find_package(PortableFileDialog REQUIRED)

add_library(Editor ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(Editor PRIVATE "Pch.hpp")
set_property(TARGET Editor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Editor Core ImGui Window Physics Renderer Game)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
  "${PROJECT_BINARY_DIR}/../.."
  ${PROJECT_BINARY_DIR}
	${Core_INCLUDES}
	${PortableFileDialog_INCLUDE_DIRS}
)

target_include_directories(Editor SYSTEM PRIVATE ${Stb_INCLUDE_DIRS})

target_compile_definitions(Editor PRIVATE HOD_EDITOR_EXPORT)
target_link_libraries(Editor
	Core
	ImGui
	Window
	Physics
	Renderer
  Game
)
