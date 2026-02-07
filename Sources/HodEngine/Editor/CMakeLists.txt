cmake_minimum_required(VERSION 3.10)

include(Global)

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

assign_source_group(${Srcs}) # for VS
set_source_files_properties(StbImplementation.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) #Build SingleFileHeader lib with Implementation define separatly

add_library(Editor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Editor ALIAS Editor)
set_target_properties(Renderer PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(Editor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
	PRIVATE ${GENERATED_DIR} SYSTEM Stb::Stb PortableFileDialogs::PortableFileDialogs
)
target_precompile_headers(Editor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Editor
	PUBLIC Core ImGui Window Physics Renderer Game
)

target_compile_definitions(Editor PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Editor PRIVATE HOD_EDITOR_EXPORT)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${SLANG_COMPILER}
		$<TARGET_FILE_DIR:Editor>/Tools/slangc.exe
	COMMENT "Copying ${SLANG_COMPILER} to output directory"
)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${SLANG_DLL}
		$<TARGET_FILE_DIR:Editor>/Tools/slang.dll
	COMMENT "Copying ${SLANG_DLL} dll to output directory"
)

add_custom_command(
	TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${SLANG_DLL_GLSLANG}
		$<TARGET_FILE_DIR:Editor>/Tools/slang-glslang.dll
	COMMENT "Copying ${SLANG_DLL_GLSLANG} dll to output directory"
)

install(TARGETS Editor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)
