project(AudioEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(AudioEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::AudioEditor ALIAS AudioEditor)
set_target_properties(AudioEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(AudioEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(AudioEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(AudioEditor
	PUBLIC Audio Editor
)

target_compile_definitions(AudioEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_AUDIO_EDITOR_EXPORT
)

hod_install_lib(AudioEditor)
