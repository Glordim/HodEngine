cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Editor)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

# Creates C resources file from files in given directory
function(create_resources input output SourcesVar IncludeVar)
	file(MAKE_DIRECTORY ${output})
	# Iterate through input files
  string(REGEX MATCH "([^/]+)$" filename ${input})
  # Replace filename spaces & extension separator for C compatibility
  string(REGEX REPLACE "\\.| |-" "_" variablename ${filename})
  # Read hex data from file
  file(READ ${input} filedata HEX)
  # Convert hex data for C compatibility
  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
  # Create output file
  file(WRITE "${output}/${filename}.cpp" "#include <cstdint>\n\nnamespace hod\n{\n\tuint8_t ${variablename}[] = {${filedata}};\n\tuint32_t ${variablename}_size = sizeof(${variablename});\n}\n")
  file(WRITE "${output}/${filename}.h" "#pragma once\n#include <cstdint>\n\nnamespace hod\n{\n\textern uint8_t ${variablename}[];\n\textern uint32_t ${variablename}_size;\n}\n")

  set(${SourcesVar} "${output}/${filename}.cpp" PARENT_SCOPE)
  set(${IncludeVar} "${output}/${filename}.h" PARENT_SCOPE)

  message("Pack shader ${input}")
endfunction()

create_resources("Icons/folder.png" "Icons" IconSrcs IconIncludes)
list(APPEND Srcs ${IconSrcs})
list(APPEND Srcs ${IconIncludes})
list(APPEND Includes ${IconIncludes})

create_resources("Icons/folder-open.png" "Icons" IconSrcs IconIncludes)
list(APPEND Srcs ${IconSrcs})
list(APPEND Srcs ${IconIncludes})
list(APPEND Includes ${IconIncludes})

create_resources("Icons/landscape.png" "Icons" IconSrcs IconIncludes)
list(APPEND Srcs ${IconSrcs})
list(APPEND Srcs ${IconIncludes})
list(APPEND Includes ${IconIncludes})

create_resources("Icons/prefab.png" "Icons" IconSrcs IconIncludes)
list(APPEND Srcs ${IconSrcs})
list(APPEND Srcs ${IconIncludes})
list(APPEND Includes ${IconIncludes})

create_resources("CMakeProjectTemplate/CMakeLists.txt" "CMakeProjectTemplate" CMakeTemplateSrcs CMakeTemplateIncludes)
list(APPEND Srcs ${CMakeTemplateSrcs})
list(APPEND Srcs ${CMakeTemplateIncludes})
list(APPEND Includes ${CMakeTemplateIncludes})

assign_source_group(${Srcs}) # for VS

find_package(Stb REQUIRED)
find_package(PortableFileDialog REQUIRED)

add_library(Editor ${LIB_TYPE} ${Srcs} ${Includes})
set_property(TARGET Editor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Editor Core ImGui Window Physics Renderer Game)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${Core_INCLUDES}
	${PortableFileDialog_INCLUDE_DIRS}
)

target_include_directories(Editor SYSTEM PRIVATE ${Stb_INCLUDE_DIRS})

target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Editor PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HOD_EXPORT)

target_link_libraries(Editor
	Core
	ImGui
	Window
	Physics
	Renderer
  Game
)
