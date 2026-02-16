cmake_minimum_required(VERSION 3.10)

include(EmbedInSource)
include(EmbedShader)
include(ClangBuildAnalyzer)

if(APPLE)
	project(ImGui LANGUAGES CXX OBJCXX)
else()
	project(ImGui LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

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
set(GENERATED_DIR ${PROJECT_BINARY_DIR}/Generated)
set(SHADER_TMP_DIR ${PROJECT_BINARY_DIR}/ShaderTmp)
set(SHADER_OUTPUT_DIR ${GENERATED_DIR}/Shader)
set(SHADER_CPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.cpp.in)
set(SHADER_HPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.hpp.in)
set(SHADER_CONFIG Debug)

set(GENERATED_SHADER_CPPS "")
set(GENERATED_SHADER_HPPS "")
EmbedShaders(ShaderSources ${SHADER_TARGET_API} ${SHADER_CONFIG} ${SHADER_CPP_TEMPLATE} ${SHADER_HPP_TEMPLATE} ${SHADER_OUTPUT_DIR} ${SHADER_TMP_DIR} GENERATED_SHADER_CPPS GENERATED_SHADER_HPPS)
list(APPEND Srcs ${GENERATED_SHADER_CPPS})
list(APPEND Includes ${GENERATED_SHADER_HPPS})

EmbedBinaryInSource("Font/MaterialDesignIcons.ttf"
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
	${GENERATED_DIR}/Font
	GeneratedFileH
	GeneratedFileCPP
)
list(APPEND Srcs ${GeneratedFileH})
list(APPEND Includes ${GeneratedFileH})
list(APPEND Srcs ${GeneratedFileCPP})

EmbedBinaryInSource("Font/Roboto-Regular.ttf"
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
	${GENERATED_DIR}/Font
	GeneratedFileH
	GeneratedFileCPP
)
list(APPEND Srcs ${GeneratedFileH})
list(APPEND Includes ${GeneratedFileH})
list(APPEND Srcs ${GeneratedFileCPP})

assign_source_group(${Srcs}) # for VS

add_library(ImGui ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::ImGui ALIAS ImGui)
set_target_properties(Renderer PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(ImGui PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
	PRIVATE ${GENERATED_DIR} ${CMAKE_CURRENT_SOURCE_DIR} SYSTEM "DearImGui"
)
target_precompile_headers(ImGui PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

if (APPLE)
	target_link_libraries(ImGui PRIVATE "-framework GameController" "-framework AppKit")
endif()

target_link_libraries(ImGui
	PUBLIC Core Renderer Window
	PRIVATE Freetype::Freetype
)

target_compile_definitions(ImGui
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_IMGUI_EXPORT
)

install(TARGETS ImGui
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(ImGui)
