cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if (APPLE)
    project(Window LANGUAGES CXX OBJCXX)
else()
    project(Window LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Window ${LIB_TYPE} ${Srcs} ${Includes})
set_property(TARGET Window PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set(UNIX_LIBS "")

if(UNIX AND NOT APPLE)
	find_package(Wayland REQUIRED)
    find_package(XkbCommon REQUIRED)
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

    set(WaylandProtocolDir "${PROJECT_SOURCE_DIR}/Desktop/Linux/Wayland/Protocols")

	add_custom_target(build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${WaylandProtocolDir})

	wayland_generate(
        "${WAYLAND_CLIENT_PKGDATADIR}/wayland.xml"
        "${WaylandProtocolDir}/wayland-client-protocol")
    wayland_generate(
        "${WAYLAND_PROTOCOLS_BASE}/stable/xdg-shell/xdg-shell.xml"
        "${WaylandProtocolDir}/wayland-xdg-shell-client-protocol")
    wayland_generate(
        "${WAYLAND_PROTOCOLS_BASE}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml"
        "${WaylandProtocolDir}/wayland-xdg-decoration-protocol")

    set(UNIX_INCLUDES ${WAYLAND_CLIENT_INCLUDE_DIR} ${WAYLAND_EGL_INCLUDE_DIR} ${XkbCommon_INCLUDE_DIRS} ${LibDecor_INCLUDE_DIRS})
    set(UNIX_LIBS ${WAYLAND_CLIENT_LIBRARIES} ${WAYLAND_EGL_LIBRARIES} ${XkbCommon_LIBRARIES} ${LibDecor_LIBRARIES})
endif()

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
endif()

add_dependencies(Window Core)
target_precompile_headers(Window PRIVATE "Pch.hpp")
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${Core_INCLUDES}
	${UNIX_INCLUDES}
)

target_link_libraries(Window
	Core
	${UNIX_LIBS}
    ${COCOA_LIBRARY}
)

target_compile_definitions(Window PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Window PRIVATE HOD_WINDOW_EXPORT)
