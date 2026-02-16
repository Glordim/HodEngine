cmake_minimum_required(VERSION 3.10)

include(ClangBuildAnalyzer)

if (APPLE)
	project(Window LANGUAGES CXX OBJCXX)
else()
	project(Window LANGUAGES CXX)
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

add_library(Window ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Window ALIAS Window)
set_target_properties(Window PROPERTIES
	FOLDER "HodEngine"
)

if(UNIX AND NOT APPLE AND NOT ANDROID)
	find_package(Wayland REQUIRED)
	find_package(LibDecor REQUIRED)
	find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)

	include(FindPkgConfig)
	pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.15)
	pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)
	pkg_get_variable(WAYLAND_CLIENT_PKGDATADIR wayland-client pkgdatadir)

	macro(wayland_generate protocol_file output_file)
		add_custom_command(OUTPUT "${output_file}.h"
			COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" client-header "${protocol_file}" "${output_file}.h"
			DEPENDS "${protocol_file}"
			VERBATIM)

		add_custom_command(OUTPUT "${output_file}.c"
			COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" private-code "${protocol_file}" "${output_file}.c"
			DEPENDS "${protocol_file}"
			VERBATIM)

		target_sources(Window PRIVATE "${output_file}.h" "${output_file}.c")
	endmacro()

	set(WaylandProtocolDir "${PROJECT_BINARY_DIR}/Desktop/Linux/Wayland/Protocols")
	file(MAKE_DIRECTORY ${WaylandProtocolDir})

	wayland_generate(
		"${WAYLAND_CLIENT_PKGDATADIR}/wayland.xml"
		"${WaylandProtocolDir}/wayland-client-protocol")
	wayland_generate(
		"${WAYLAND_PROTOCOLS_BASE}/stable/xdg-shell/xdg-shell.xml"
		"${WaylandProtocolDir}/wayland-xdg-shell-client-protocol")
	wayland_generate(
		"${WAYLAND_PROTOCOLS_BASE}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml"
		"${WaylandProtocolDir}/wayland-xdg-decoration-protocol")

	target_link_libraries(Window PRIVATE ${WAYLAND_CLIENT_LIBRARIES} ${WAYLAND_EGL_LIBRARIES} ${LibDecor_LIBRARIES})
	target_include_directories(Window PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIR} ${WAYLAND_EGL_INCLUDE_DIR} ${LibDecor_INCLUDE_DIRS})
endif()

if (APPLE)
	find_library(COCOA_LIBRARY Cocoa)
	target_link_libraries(Window PRIVATE ${COCOA_LIBRARY})
endif()

if(ANDROID)
	find_library(android-lib android)
	target_link_libraries(Window PRIVATE ${android-lib})
endif()

target_include_directories(Window PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Window PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Window
	PUBLIC Core
)

target_compile_definitions(Window
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_WINDOW_EXPORT
)

install(TARGETS Window
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(Window)
