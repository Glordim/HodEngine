cmake_minimum_required(VERSION 3.10)

include(Global)
include(EmbedInSource)

if(APPLE)
project(ImGui LANGUAGES CXX OBJCXX)
else()
project(ImGui LANGUAGES CXX)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

function(CompileReflectEmbedShader INPUT_SLANG TARGET_API SHADER_TYPE SHADER_CONFIG TEMPLATE_CPP TEMPLATE_HPP OUTPUT_CPP OUTPUT_HPP TEMP_DIR)
	add_custom_command(
		OUTPUT ${OUTPUT_CPP} ${OUTPUT_HPP}
		COMMAND ${Python3_EXECUTABLE}
			${CMAKE_SOURCE_DIR}/CMake/CompileReflectEmbedShader.py
			${SLANG_COMPILER}
			${INPUT_SLANG}
			${TARGET_API}
			${SHADER_TYPE}
			${SHADER_CONFIG}
			${TEMPLATE_CPP}
			${TEMPLATE_HPP}
			${OUTPUT_CPP}
			${OUTPUT_HPP}
			${TEMP_DIR}
		DEPENDS
			${INPUT_SLANG}
			${CMAKE_SOURCE_DIR}/CMake/CompileReflectEmbedShader.py
			${TEMPLATE_CPP}
			${TEMPLATE_HPP}
		COMMENT "Compiling ${SHADER_TYPE} shader from ${INPUT_SLANG}"
	)
endfunction()

set(ShaderCpps "")
set(ShaderHpps "")

file(GLOB_RECURSE ShaderSources "${PROJECT_SOURCE_DIR}/Shader/*.slang")

if(NOT APPLE)
	if (WIN32)
		#set(SHADER_TARGET_API d3d12)
		set(SHADER_TARGET_API Vulkan)
	else()
		set(SHADER_TARGET_API Vulkan)
	endif()
else()
	set(SHADER_TARGET_API Metal)
endif()
set(SHADER_TMP_DIR ${PROJECT_BINARY_DIR}/ShaderTmp)
set(SHADER_CPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in)
set(SHADER_HPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in)

set(SHADER_CONFIG Debug)

foreach(file ${ShaderSources})
	get_filename_component(BASENAME "${file}" NAME_WE)

	set(SHADER_TYPE "Vertex")
	set(SHADER_EMBED_CPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.cpp)
	set(SHADER_EMBED_HPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.hpp)
	CompileReflectEmbedShader(${file} ${SHADER_TARGET_API} ${SHADER_TYPE} ${SHADER_CONFIG} ${SHADER_CPP_TEMPLATE} ${SHADER_HPP_TEMPLATE} ${SHADER_EMBED_CPP} ${SHADER_EMBED_HPP} ${SHADER_TMP_DIR})
	list(APPEND ShaderCpps ${SHADER_EMBED_CPP})
	list(APPEND ShaderHpps ${SHADER_EMBED_HPP})

	set(SHADER_TYPE "Fragment")
	set(SHADER_EMBED_CPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.cpp)
	set(SHADER_EMBED_HPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.hpp)
	CompileReflectEmbedShader(${file} ${SHADER_TARGET_API} ${SHADER_TYPE} ${SHADER_CONFIG} ${SHADER_CPP_TEMPLATE} ${SHADER_HPP_TEMPLATE} ${SHADER_EMBED_CPP} ${SHADER_EMBED_HPP} ${SHADER_TMP_DIR})
	list(APPEND ShaderCpps ${SHADER_EMBED_CPP})
	list(APPEND ShaderHpps ${SHADER_EMBED_HPP})
endforeach()

list(APPEND Srcs ${ShaderCpps})
list(APPEND Includes ${ShaderHpps})

EmbedBinaryInSource("Font/MaterialDesignIcons.ttf"
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
	${PROJECT_BINARY_DIR}/Font
	GeneratedFileH
	GeneratedFileCPP
)
list(APPEND Srcs ${GeneratedFileH})
list(APPEND Includes ${GeneratedFileH})
list(APPEND Srcs ${GeneratedFileCPP})

EmbedBinaryInSource("Font/Roboto-Regular.ttf"
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
	${PROJECT_BINARY_DIR}/Font
	GeneratedFileH
	GeneratedFileCPP
)
list(APPEND Srcs ${GeneratedFileH})
list(APPEND Includes ${GeneratedFileH})
list(APPEND Srcs ${GeneratedFileCPP})

assign_source_group(${Srcs}) # for VS

add_library(ImGui ${LIB_TYPE} ${Srcs} ${Includes})
set_target_properties(ImGui PROPERTIES FOLDER "HodEngine")
target_precompile_headers(ImGui PRIVATE "Pch.hpp")
set_property(TARGET ImGui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(ImGui Core Window Renderer Freetype::Freetype)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)
get_target_property(Renderer_INCLUDES Renderer INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  "${PROJECT_BINARY_DIR}/../.."
  ${PROJECT_BINARY_DIR}
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Renderer_INCLUDES}
)

include_directories(SYSTEM
  "DearImGui" # for imguiFreetype include
)

if (APPLE)
	set(PLATFORM_LIBS "-framework GameController" "-framework AppKit")
endif()

target_link_libraries(ImGui PUBLIC
  Core
  Renderer
  Window
)

target_link_libraries(ImGui PRIVATE
  Freetype::Freetype
  ${PLATFORM_LIBS}
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(ImGui PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(ImGui PRIVATE HOD_IMGUI_EXPORT)
