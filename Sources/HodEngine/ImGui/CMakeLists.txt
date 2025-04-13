cmake_minimum_required(VERSION 3.10)

include(Global)
include(EmbedInSource)

if(APPLE)
project(ImGui LANGUAGES CXX OBJCXX)
else()
project(ImGui LANGUAGES CXX)
endif()

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

file(GLOB_RECURSE ShaderSources
	"${PROJECT_SOURCE_DIR}/Shader/*"
)

foreach(file ${ShaderSources})
  EmbedTextInSource(${file}
  ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/TextSourceFileTemplate.hpp.in
  ${PROJECT_BINARY_DIR}/Shader
  GeneratedFileH
  )
  list(APPEND Srcs ${GeneratedFileH})
  list(APPEND Includes ${GeneratedFileH})
endforeach()

EmbedBinaryInSource("Font/MaterialDesignIcons.ttf"
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
	${PROJECT_BINARY_DIR}/Font
	GeneratedFileH
	GeneratedFileCPP
)
list(APPEND Srcs ${GeneratedFileH})
list(APPEND Includes ${GeneratedFileH})
list(APPEND Srcs ${GeneratedFileCPP})

EmbedBinaryInSource("Font/Roboto-Regular.ttf"
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in
	${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in
	${PROJECT_BINARY_DIR}/Font
	GeneratedFileH
	GeneratedFileCPP
)
list(APPEND Srcs ${GeneratedFileH})
list(APPEND Includes ${GeneratedFileH})
list(APPEND Srcs ${GeneratedFileCPP})

assign_source_group(${Srcs}) # for VS

add_library(ImGui ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(ImGui PRIVATE "Pch.hpp")
set_property(TARGET ImGui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(ImGui Core Window Renderer freetype)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)
get_target_property(Renderer_INCLUDES Renderer INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  "${PROJECT_BINARY_DIR}/../.."
  ${PROJECT_BINARY_DIR}
  ${Core_INCLUDES}
  ${Window_INCLUDES}
  ${Renderer_INCLUDES}
  ${freetype_INCLUDE_DIRS}
)

include_directories(SYSTEM
  "DearImGui" # for imguiFreetype include
)

target_link_libraries(ImGui
  Core
  Renderer
  Window
  ${freetype_LIBRARIES}
)

if (APPLE)
target_link_libraries(ImGui "-framework GameController")
target_link_libraries(ImGui "-framework AppKit")
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(ImGui PUBLIC ${PLATFORM_DEFINE})
target_compile_definitions(ImGui PRIVATE HOD_IMGUI_EXPORT)
