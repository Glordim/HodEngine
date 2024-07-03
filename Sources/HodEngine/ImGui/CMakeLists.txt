cmake_minimum_required(VERSION 3.4...3.27)

include(Global)


if(APPLE)
project(ImGui LANGUAGES CXX OBJCXX)
else()
project(ImGui LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

file(GLOB_RECURSE ShaderSources
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.vert"
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.frag"
)

set(ShaderGenerated "")
foreach(file ${ShaderSources})
	string(REPLACE "${PROJECT_SOURCE_DIR}/Shader/Sources/" "${PROJECT_SOURCE_DIR}/Shader/Generated/" relocated_file "${file}")
    list(APPEND ShaderGenerated "${relocated_file}.cpp")
    list(APPEND ShaderGenerated "${relocated_file}.hpp")
	list(APPEND Srcs "${relocated_file}.cpp")
	list(APPEND Srcs "${relocated_file}.hpp")
	list(APPEND Includes "${relocated_file}.hpp")
endforeach()

add_custom_command(OUTPUT ${ShaderGenerated}
    COMMAND HodShaderConverter --input Shader/Sources --output Shader/Generated
    DEPENDS HodShaderConverter ${ShaderSources}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating shader source files"
)

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

create_resources("Font/MaterialDesignIcons.ttf" "Font" FontSrcs FontIncludes)
list(APPEND Srcs ${FontSrcs})
list(APPEND Srcs ${FontIncludes})
list(APPEND Includes ${FontIncludes})

create_resources("Font/Roboto-Regular.ttf" "Font" FontSrcs FontIncludes)
list(APPEND Srcs ${FontSrcs})
list(APPEND Srcs ${FontIncludes})
list(APPEND Includes ${FontIncludes})

assign_source_group(${Srcs}) # for VS

add_library(ImGui STATIC ${Srcs} ${Includes})
set_property(TARGET ImGui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(ImGui Core Window Renderer freetype)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)
get_target_property(Renderer_INCLUDES Renderer INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  "DearImGui" # for imguiFreetype include
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Renderer_INCLUDES}
  ${freetype_INCLUDE_DIRS}
)

target_link_libraries(ImGui
  Core
  Renderer
  Window
  ${freetype_LIBRARIES}
)

if (APPLE)
target_link_libraries(ImGui "-framework GameController")
target_link_libraries(ImGui "-framework AppKit")
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(ImGui PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HOD_EXPORT)
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HOD_EXPORT)
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HOD_EXPORT)
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HOD_EXPORT)
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HOD_EXPORT)
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HOD_EXPORT)
