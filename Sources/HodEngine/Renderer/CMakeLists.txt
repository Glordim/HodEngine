cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(Renderer)

find_package(Vulkan REQUIRED)
find_package(SpirvCross REQUIRED)
find_package(Stb REQUIRED)

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
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")
