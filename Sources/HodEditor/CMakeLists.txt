project(HodEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS
if (WIN32)
	list(APPEND Srcs "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")
elseif (APPLE)
	set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/icon.icns")
	set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	list(APPEND Srcs ${ICON_FILE})
endif()

add_executable(HodEditor ${Srcs} ${Includes})
set_target_properties(HodEditor PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
	VS_STARTUP_PROJECT "HodEditor"
)
if (APPLE)
	set_target_properties(HodEditor PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_ICON_FILE icon.icns
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.hodengine.hodeditor"
		MACOSX_BUNDLE_BUNDLE_NAME "HodEditor"
	)
endif()

target_include_directories(HodEditor
	PRIVATE "../.."
)
#target_precompile_headers(HodEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

add_dependencies(HodEditor
	Application
	Editor
	CoreEditor
	InputEditor
	GameEditor
	UIEditor
	UI2Editor
)

target_link_libraries(HodEditor
	PUBLIC Application Editor
)

target_compile_definitions(HodEditor PUBLIC ${HOD_PLATFORM_DEFINE})

install(TARGETS HodEditor
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	BUNDLE DESTINATION bin
	INCLUDES DESTINATION include
)
