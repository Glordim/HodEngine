cmake_minimum_required(VERSION 3.10)

project(Main LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Main ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Main ALIAS Main)

if(ANDROID)
	find_library(android-lib android)
	target_link_libraries(Main PUBLIC
		${android-lib}
		log
		-Wl,--whole-archive
		AGDK::GameActitvity
		-Wl,--no-whole-archive
	)
endif()

set_target_properties(Main PROPERTIES
	FOLDER "HodEngine"
)
target_include_directories(Main PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)

target_link_libraries(Main
	PUBLIC HodEngine::Application
)

target_compile_definitions(Main PUBLIC ${HOD_PLATFORM_DEFINE})

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Main PUBLIC HOD_MAIN_STATIC)
else()
	target_compile_definitions(Main PRIVATE HOD_MAIN_EXPORT)
endif()

hod_install_lib(Main)
