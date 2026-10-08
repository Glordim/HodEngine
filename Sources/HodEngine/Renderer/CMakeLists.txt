include(EmbedInSource)
include(EmbedShader)

project(Renderer LANGUAGES CXX)

get_target_property(API RHI HOD_RHI_BACKEND)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

file(GLOB_RECURSE ShaderSources "${PROJECT_SOURCE_DIR}/Shader/*.slang")

set(GENERATED_DIR ${PROJECT_BINARY_DIR}/Generated)
set(SHADER_TMP_DIR ${PROJECT_BINARY_DIR}/ShaderTmp)
set(SHADER_OUTPUT_DIR ${GENERATED_DIR}/Shader)
set(SHADER_CPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.cpp.in)
set(SHADER_HPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.hpp.in)
set(SHADER_CONFIG Debug)

set(GENERATED_SHADER_CPPS "")
set(GENERATED_SHADER_HPPS "")
EmbedShaders(ShaderSources ${API} ${SHADER_CONFIG} ${SHADER_CPP_TEMPLATE} ${SHADER_HPP_TEMPLATE} ${SHADER_OUTPUT_DIR} ${SHADER_TMP_DIR} GENERATED_SHADER_CPPS GENERATED_SHADER_HPPS)
list(APPEND Srcs ${GENERATED_SHADER_CPPS})
list(APPEND Includes ${GENERATED_SHADER_HPPS})

assign_source_group(${Srcs})

add_library(Renderer ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Renderer ALIAS Renderer)
set_target_properties(Renderer PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(Renderer PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
	PRIVATE ${GENERATED_DIR}
)
target_precompile_headers(Renderer PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Renderer
	PUBLIC Core Math Window GameSystems RHI
	PRIVATE Freetype::Freetype
)

if(APPLE)
	find_package(BZip2 REQUIRED)
	find_package(ZLIB REQUIRED)
	target_link_libraries(Renderer PRIVATE ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES})
endif()

target_compile_definitions(Renderer PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Renderer PUBLIC HOD_RENDERER_STATIC)
else()
	target_compile_definitions(Renderer PRIVATE HOD_RENDERER_EXPORT)
endif()

hod_install_lib(Renderer)
