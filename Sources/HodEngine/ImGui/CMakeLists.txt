cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

project(ImGui)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

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

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(ImGui PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>")
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>")
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>")
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>")
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>")
target_compile_definitions(ImGui PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>")
