cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if (APPLE)
    project(Window LANGUAGES CXX OBJCXX)
else()
    project(Window LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Audio ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(Audio PRIVATE "Pch.hpp")
set_property(TARGET Audio PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Audio Core)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"../.."
	${Core_INCLUDES}
)

target_link_libraries(Audio
	Core
)

target_compile_definitions(Audio PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(Audio PRIVATE HOD_AUDIO_EXPORT)
