cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Renderer)

find_package(Vulkan REQUIRED)
find_package(SpirvCross REQUIRED)
find_package(Stb REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.inl"
)

file(GLOB_RECURSE Includes
"${PROJECT_SOURCE_DIR}/*.h"
"${PROJECT_SOURCE_DIR}/*.inl"
)

file(GLOB_RECURSE ShaderSources
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.vert"
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.frag"
)

set(ShaderGenerated "")
foreach(file ${ShaderSources})
	string(REPLACE "${PROJECT_SOURCE_DIR}/Shader/Sources/" "${PROJECT_SOURCE_DIR}/Shader/Generated/" relocated_file "${file}")
    list(APPEND ShaderGenerated "${relocated_file}.cpp")
    list(APPEND ShaderGenerated "${relocated_file}.h")
	list(APPEND Srcs "${relocated_file}.cpp")
	list(APPEND Srcs "${relocated_file}.h")
	list(APPEND Includes "${relocated_file}.h")
endforeach()

add_custom_command(OUTPUT ${ShaderGenerated}
    COMMAND HodShaderConverter --input Shader/Sources --output Shader/Generated
    DEPENDS HodShaderConverter ${ShaderSources}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating shader source files"
)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
"../.."
${Stb_INCLUDE_DIRS}
${SpirvCross_INCLUDE_DIRS}
${Vulkan_INCLUDE_DIRS}
)

add_library(Renderer OBJECT ${Srcs} ${Includes})
set_property(TARGET Renderer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Renderer Core Window)

target_link_libraries(Renderer
  Core
  Window
  #optimized "d3d12.lib" debug "d3d12.lib"
  #optimized "DXGI.lib" debug "DXGI.lib"
  ${Vulkan_LIBRARIES}
  ${SpirvCross_LIBRARIES}
)

option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" True)
#set(RENDERER_ENABLE_VALIDATION_LAYER True)

if (RENDERER_ENABLE_VALIDATION_LAYER)
	set(RENDERER_ENABLE_VALIDATION_LAYER_DEFINE "RENDERER_ENABLE_VALIDATION_LAYER")
endif ()

target_compile_definitions(Renderer PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE} ${RENDERER_ENABLE_VALIDATION_LAYER_DEFINE})

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

#create_resources("Shader/Sources" "Shader/Generated")