cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(ImGui)

# Creates C resources file from files in given directory
function(create_resources dir output)
	file(MAKE_DIRECTORY ${output})
	# Collect input files
	file(GLOB bins ${dir}/*)
	# Iterate through input files
	foreach(bin ${bins})
		string(REGEX MATCH "([^/]+)$" filename ${bin})
		# Replace filename spaces & extension separator for C compatibility
		string(REGEX REPLACE "\\.| |-" "_" variablename ${filename})
		# Read hex data from file
		file(READ ${bin} filedata HEX)
		# Convert hex data for C compatibility
		string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
		# Create output file
		file(WRITE "${output}/${filename}.cpp" "#include <cstdint>\n\nnamespace hod\n{\n\tuint8_t ${variablename}[] = {${filedata}};\n\tuint32_t ${variablename}_size = sizeof(${variablename});\n}\n")
		file(WRITE "${output}/${filename}.h" "#pragma once\n#include <cstdint>\n\nnamespace hod\n{\n\textern uint8_t ${variablename}[];\n\textern uint32_t ${variablename}_size;\n}\n")

		message("Pack shader ${bin}")

		execute_process(COMMAND ${CMAKE_COMMAND} --build . --target HodShaderConverter --input ${bin} --output ${output}/${filename} --embeddedSource WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/HodShaderConverter RESULT_VARIABLE CommandResult)

		if(CommandResult EQUAL 0)
			#message(STATUS "Liste des fichiers : ${FILE_LIST}")
			message(STATUS "Done")
		else()
			message(FATAL_ERROR "Fail: '${CommandResult}'")
		endif()
	endforeach()
endfunction()

#create_resources("Shader/Sources" "Shader/Generated")

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.inl"
)

file(GLOB_RECURSE Includes
"${PROJECT_SOURCE_DIR}/*.h"
"${PROJECT_SOURCE_DIR}/*.inl"
)

assign_source_group(${Srcs}) # for VS

add_library(ImGui OBJECT ${Srcs} ${Includes})
set_property(TARGET ImGui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_custom_target(ImGui_GenerateEmbeddedShader
    COMMAND HodShaderConverter --input Shader/Sources --output Shader/Generated
    DEPENDS HodShaderConverter
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating shader source files"
)

add_dependencies(ImGui Core Window Renderer ImGui_GenerateEmbeddedShader)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)
get_target_property(Renderer_INCLUDES Renderer INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Renderer_INCLUDES}
)

target_link_libraries(ImGui
  Core
  Renderer
  Window
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(ImGui PUBLIC ${PLATFORM_DEFINE})
