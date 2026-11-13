project(VersionControlEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(VersionControlEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::VersionControlEditor ALIAS VersionControlEditor)
set_target_properties(VersionControlEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(VersionControlEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(VersionControlEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(VersionControlEditor
	PUBLIC Core Editor
)

target_compile_definitions(VersionControlEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_VERSION_CONTROL_EDITOR_EXPORT
)

hod_install_lib(VersionControlEditor)
hod_plugin_manifest(VersionControlEditor VersionControl "${CMAKE_CURRENT_SOURCE_DIR}/HodPlugin.json")
