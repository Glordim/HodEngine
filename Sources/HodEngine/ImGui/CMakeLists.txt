cmake_minimum_required(VERSION 3.4...3.27)

include(Global)


if(APPLE)
project(ImGui LANGUAGES CXX OBJCXX)
else()
project(ImGui LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

function(EmbeedShaderSource InputFile TemplateFile OutputDir OutputFile)
  message(STATUS "Pack Shader: ${InputFile}")

  get_filename_component(FILENAME "${InputFile}" NAME)
  string(REPLACE "." "_" ESCAPED_FILE_NAME "${FILENAME}")

  file(READ "${InputFile}" FILE_CONTENTS)
  string(REPLACE "\"" "\\\"" ESCAPED_FILE_CONTENTS "${FILE_CONTENTS}")
  string(REPLACE "\n" "\\n\"\n\"" ESCAPED_FILE_CONTENTS "${ESCAPED_FILE_CONTENTS}")

  configure_file(
      ${TemplateFile}
      ${OutputDir}/${FILENAME}.hpp
      @ONLY
  )
endfunction()

file(GLOB_RECURSE ShaderSources
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.vert"
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.frag"
)

foreach(file ${ShaderSources})
  EmbeedShaderSource(${file} ${PROJECT_SOURCE_DIR}/Shader/Template/ShaderSourceFileTemplate.hpp.in ${PROJECT_SOURCE_DIR}/Shader/Generated GeneratedFile)
  list(APPEND Srcs ${GeneratedFile})
  list(APPEND Includes ${GeneratedFile})
endforeach()

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

add_library(ImGui ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(ImGui PRIVATE "Pch.hpp")
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
target_compile_definitions(ImGui PRIVATE HOD_IMGUI_EXPORT)
