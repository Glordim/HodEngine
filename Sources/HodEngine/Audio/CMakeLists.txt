if (APPLE)
	project(Audio LANGUAGES CXX OBJCXX)
else()
	project(Audio LANGUAGES CXX)
endif()

if (APPLE)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES Win32 Android Linux)
elseif(ANDROID)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES MacOs Win32 Linux)
elseif(WIN32)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES MacOs Android Linux)
else() # Linux
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES Win32 Android MacOs)
endif()

assign_source_group(${Srcs}) # for VS

add_library(Audio ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Audio ALIAS Audio)
set_target_properties(Audio PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(Audio PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Audio PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

# An optional module (see HodModule.json): Game doesn't know it, it registers its components in Game
target_link_libraries(Audio
	PUBLIC Core GameSystems Game
)

if(ANDROID)
	target_link_libraries(Audio PRIVATE aaudio)
endif()

target_compile_definitions(Audio PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Audio PUBLIC HOD_AUDIO_STATIC)
else()
	target_compile_definitions(Audio PRIVATE HOD_AUDIO_EXPORT)
endif()

hod_install_lib(Audio)
hod_module_manifest(Audio Audio "${CMAKE_CURRENT_SOURCE_DIR}/HodModule.json")
