cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(ImGui)

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

add_dependencies(ImGui Core Window Renderer)
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

target_compile_definitions(ImGui PUBLIC ${PLATFORM_DEFINE})

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
		file(WRITE "${output}/${filename}.cpp" "#include <cstdint>\n\nnamespace hod\n{\n\tconst uint8_t ${variablename}[] = {${filedata}};\n\tconst uint32_t ${variablename}_size = sizeof(${variablename});\n}\n")
		file(WRITE "${output}/${filename}.h" "#include <cstdint>\n\nnamespace hod\n{\n\tstatic uint8_t* ${variablename};\n\tstatic const uint32_t ${variablename}_size;\n}\n")
	endforeach()
endfunction()

create_resources("Shader/Sources" "Shader/Generated")
