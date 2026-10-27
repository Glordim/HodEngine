project(RendererEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

set(GENERATED_DIR ${PROJECT_BINARY_DIR}/Generated)

# The shaders of the previews (texture...), compiled for the graphics API of the RHI
include(EmbedShader)
file(GLOB_RECURSE ShaderSources "${PROJECT_SOURCE_DIR}/Shader/*.slang")
get_target_property(SHADER_TARGET_API RHI HOD_RHI_BACKEND)
set(GENERATED_SHADER_CPPS "")
set(GENERATED_SHADER_HPPS "")
EmbedShaders(ShaderSources ${SHADER_TARGET_API} Debug
	${CMAKE_SOURCE_DIR}/Sources/HodEngine/Editor/EmbedInSourceTemplate/ShaderSourceFileTemplate.cpp.in
	${CMAKE_SOURCE_DIR}/Sources/HodEngine/Editor/EmbedInSourceTemplate/ShaderSourceFileTemplate.hpp.in
	${GENERATED_DIR}/Shader
	${PROJECT_BINARY_DIR}/ShaderTmp
	GENERATED_SHADER_CPPS
	GENERATED_SHADER_HPPS
)
list(APPEND Srcs ${GENERATED_SHADER_CPPS})
list(APPEND Includes ${GENERATED_SHADER_HPPS})

assign_source_group(${Srcs}) # for VS
set_source_files_properties(StbImplementation.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) #Build SingleFileHeader lib with Implementation define separatly

add_library(RendererEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::RendererEditor ALIAS RendererEditor)
set_target_properties(RendererEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(RendererEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
	PRIVATE ${GENERATED_DIR}
)
target_precompile_headers(RendererEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(RendererEditor
	PUBLIC Renderer Text Editor CoreEditor
	PRIVATE Stb::Stb
)

target_compile_definitions(RendererEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_RENDERER_EDITOR_EXPORT
)

# The materials are cooked for the graphics API of the RHI (slangc and its tools are copied next to the editor by Editor)
get_target_property(RHI_BACKEND RHI HOD_RHI_BACKEND)
if(RHI_BACKEND STREQUAL "D3D12")
	target_compile_definitions(RendererEditor PRIVATE HOD_SHADER_TARGET_D3D12)
endif()

hod_install_lib(RendererEditor)
