if (APPLE)
	project(Application LANGUAGES CXX OBJCXX)
else()
	project(Application LANGUAGES CXX)
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

add_library(Application ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Application ALIAS Application)
set_target_properties(Application PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(Application PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Application PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Application
	PUBLIC Core Audio Window Renderer Input Physics Game ImGui UI UI2
)

if (ANDROID)
	find_library(android-lib android)
	target_link_libraries(Application PRIVATE ${android-lib} log AGDK::GameActitvity)
endif()

target_compile_definitions(Application PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Application PUBLIC HOD_APPLICATION_STATIC)
else()
	target_compile_definitions(Application PRIVATE HOD_APPLICATION_EXPORT)
endif()

hod_install_lib(Application)
