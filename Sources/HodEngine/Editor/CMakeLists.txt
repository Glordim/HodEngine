project(Editor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES FileTemplate)

file(GLOB_RECURSE Icons
	"${PROJECT_SOURCE_DIR}/Icons/*.png"
)

set(GENERATED_DIR ${PROJECT_BINARY_DIR}/Generated)

foreach(file ${Icons})
	EmbedBinaryInSource(${file}
		${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
		${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
		${GENERATED_DIR}/Icons
		GeneratedFileH
		GeneratedFileCPP
	)
	list(APPEND Srcs ${GeneratedFileH})
	list(APPEND Includes ${GeneratedFileH})
	list(APPEND Srcs ${GeneratedFileCPP})
endforeach()

file(GLOB_RECURSE FileTemplates
	"${PROJECT_SOURCE_DIR}/FileTemplate/*"
)

foreach(file ${FileTemplates})
	EmbedTextInSource(${file}
		${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/TextSourceFileTemplate.hpp.in
		${GENERATED_DIR}/FileTemplates
		GeneratedFileH
	)
	list(APPEND Srcs ${GeneratedFileH})
	list(APPEND Includes ${GeneratedFileH})
endforeach()

# The shaders of the editor itself (previews...), compiled for the graphics API of the RHI
include(EmbedShader)
file(GLOB_RECURSE ShaderSources "${PROJECT_SOURCE_DIR}/Shader/*.slang")
get_target_property(SHADER_TARGET_API RHI HOD_RHI_BACKEND)
set(GENERATED_SHADER_CPPS "")
set(GENERATED_SHADER_HPPS "")
EmbedShaders(ShaderSources ${SHADER_TARGET_API} Debug
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.cpp.in
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.hpp.in
	${GENERATED_DIR}/Shader
	${PROJECT_BINARY_DIR}/ShaderTmp
	GENERATED_SHADER_CPPS
	GENERATED_SHADER_HPPS
)
list(APPEND Srcs ${GENERATED_SHADER_CPPS})
list(APPEND Includes ${GENERATED_SHADER_HPPS})

assign_source_group(${Srcs}) # for VS
set_source_files_properties(StbImplementation.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) #Build SingleFileHeader lib with Implementation define separatly

add_library(Editor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Editor ALIAS Editor)
set_target_properties(Renderer PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(Editor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
	PRIVATE ${GENERATED_DIR}
)
target_precompile_headers(Editor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Editor
	PUBLIC Core ImGui Window Physics Renderer Game Audio UI2
	PRIVATE Stb::Stb PortableFileDialogs::PortableFileDialogs
)

if(NOT BUILD_RETAIL)
	target_compile_definitions(Editor PRIVATE HOD_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
endif()

target_compile_definitions(Editor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_EDITOR_EXPORT
)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/AndroidStudioTemplate"
		"$<TARGET_FILE_DIR:Editor>/Templates/AndroidStudio"
	COMMENT "Copying AndroidStudioTemplate to output directory"
)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:slang::slangc>
		$<TARGET_FILE_DIR:Editor>/Tools/slangc.exe
	COMMENT "Copying slangc to output directory"
)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:slang::slang>
		$<TARGET_FILE_DIR:Editor>/Tools/$<TARGET_FILE_NAME:slang::slang>
	COMMENT "Copying slang runtime to output directory"
)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:slang::slang-glslang>
		$<TARGET_FILE_DIR:Editor>/Tools/$<TARGET_FILE_NAME:slang::slang-glslang>
	COMMENT "Copying slang-glslang to output directory"
)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/Sources/HodEngine/Renderer/ShaderLib"
		"$<TARGET_FILE_DIR:Editor>/Tools/ShaderLib"
	COMMENT "Copying ShaderLib to output directory"
)

# The materials are cooked for the graphics API of the RHI
get_target_property(RHI_BACKEND RHI HOD_RHI_BACKEND)
if(RHI_BACKEND STREQUAL "D3D12")
	include(EmbedShader)
	FindDxc()

	target_compile_definitions(Editor PRIVATE HOD_SHADER_TARGET_D3D12)

	# slangc emits DXIL through dxcompiler.dll and signs it with dxil.dll, which it looks for next to itself
	add_custom_command(
		TARGET Editor POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${HOD_DXC_DIR}/dxcompiler.dll"
			"${HOD_DXC_DIR}/dxil.dll"
			$<TARGET_FILE_DIR:Editor>/Tools
		COMMENT "Copying the DirectX Shader Compiler to output directory"
	)
endif()

hod_install_lib(Editor)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/AndroidStudioTemplate/"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/${HOD_LIB_TYPE_SUBDIR}/bin/Templates/AndroidStudio"
)
