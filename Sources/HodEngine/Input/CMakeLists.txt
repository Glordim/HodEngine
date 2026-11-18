if (APPLE)
	project(Input LANGUAGES CXX OBJCXX)
else()
	project(Input LANGUAGES CXX)
endif()

if (WIN32)
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)
else()
	CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES RawInput XInput)
endif()

assign_source_group(${Srcs}) # for VS

add_library(Input ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Input ALIAS Input)
set_target_properties(Input PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(Input PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Input PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

if (APPLE)
	target_link_libraries(Input PRIVATE "-framework GameController")
endif()
if (WIN32)
	target_link_libraries(Input PRIVATE WindowsHModular::WindowsHModular GameInput::GameInput)

	# GameInput's runtime next to the engine's executables (the editor, the tests), the applications' by hod_deploy_runtime_files
	if(BUILD_SHARED_LIBS)
		add_custom_command(TARGET Input POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GAMEINPUT_REDIST_DLL}" "$<TARGET_FILE_DIR:Input>/"
			COMMENT "Copying GameInput's runtime to output directory"
		)
		install(FILES "${GAMEINPUT_REDIST_DLL}"
			DESTINATION "${HOD_PLATFORM_SUBDIR}/${HOD_LIB_TYPE_SUBDIR}/bin"
		)
	endif()
endif()
if(ANDROID)
	find_library(android-lib android)
	target_link_libraries(Input PRIVATE ${android-lib} AGDK::GameActitvity)
endif()

target_link_libraries(Input
	PUBLIC Core Math Window GameSystems
)

if(APPLE)
	# Each .mm file auto-links libobjc via an embedded linker directive;
	# with several .mm files in this target, Xcode's linker sees "-lobjc"
	# more than once and warns. CMP0156 doesn't cover this (it only
	# de-duplicates libraries CMake itself puts on the link line).
	target_link_options(Input PRIVATE "-Wl,-no_warn_duplicate_libraries")
endif()

target_compile_definitions(Input PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Input PUBLIC HOD_INPUT_STATIC)
else()
	target_compile_definitions(Input PRIVATE HOD_INPUT_EXPORT)
endif()

hod_install_lib(Input)
