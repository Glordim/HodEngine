project(AudioComponents LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(AudioComponents ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::AudioComponents ALIAS AudioComponents)
set_target_properties(AudioComponents PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(AudioComponents PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(AudioComponents PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

# The Components layer of the Audio plugin (see HodPlugin.json in Audio): the only one that knows Game (the AudioSourceComponent, the
# runtime module's entry point), over the Audio library that doesn't
target_link_libraries(AudioComponents
	PUBLIC Audio Game
)

target_compile_definitions(AudioComponents PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(AudioComponents PUBLIC HOD_AUDIO_COMPONENTS_STATIC)
else()
	target_compile_definitions(AudioComponents PRIVATE HOD_AUDIO_COMPONENTS_EXPORT)
endif()

hod_install_lib(AudioComponents)
