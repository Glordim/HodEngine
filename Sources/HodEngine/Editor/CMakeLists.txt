cmake_minimum_required(VERSION 3.10)

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
)

target_include_directories(Editor SYSTEM PRIVATE Stb::Stb PortableFileDialogs::PortableFileDialogs)

target_compile_definitions(Editor PRIVATE HOD_EDITOR_EXPORT)
target_link_libraries(Editor
	Core
	ImGui
	Window
	Physics
	Renderer
  Game
)

add_custom_command(
  TARGET Editor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SLANG_COMPILER}
    $<TARGET_FILE_DIR:Editor>/Tools/slangc.exe
  COMMENT "Copying SLANG_COMPILER to output directory"
)

add_custom_command(
  TARGET Editor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SLANG_DLL}
    $<TARGET_FILE_DIR:Editor>/Tools/slang.dll
  COMMENT "Copying SLANG_COMPILER dll to output directory"
)

add_custom_command(
  TARGET Editor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SLANG_DLL_GLSLANG}
    $<TARGET_FILE_DIR:Editor>/Tools/slang-glslang.dll
  COMMENT "Copying SLANG_COMPILER dll to output directory"
)
