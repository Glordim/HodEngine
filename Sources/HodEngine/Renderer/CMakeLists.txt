cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if(NOT APPLE)
project(Renderer LANGUAGES CXX)
find_package(Vulkan REQUIRED)
find_package(SpirvCross REQUIRED)
else()
project(Renderer LANGUAGES CXX OBJCXX)
find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
endif()
find_package(Stb REQUIRED)

if(NOT APPLE)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES D3d12 Metal)
else()
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES ".*/RHI/Vulkan/.*")
endif()

file(GLOB_RECURSE ShaderSources
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.vert"
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.frag"
)

set(ShaderGenerated "")
foreach(file ${ShaderSources})
	string(REPLACE "${PROJECT_SOURCE_DIR}/Shader/Sources/" "${PROJECT_SOURCE_DIR}/Shader/Generated/" relocated_file "${file}")
    list(APPEND ShaderGenerated "${relocated_file}.cpp")
    list(APPEND ShaderGenerated "${relocated_file}.hpp")
	list(APPEND Srcs "${relocated_file}.cpp")
	list(APPEND Srcs "${relocated_file}.hpp")
	list(APPEND Includes "${relocated_file}.hpp")
endforeach()

add_custom_command(OUTPUT ${ShaderGenerated}
    COMMAND HodShaderConverter --input Shader/Sources --output Shader/Generated
    DEPENDS HodShaderConverter ${ShaderSources}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating shader source files"
)

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
"../.."
${Stb_INCLUDE_DIRS}
${msdfgen_INCLUDE_DIRS}
${freetype_INCLUDE_DIRS}
"RHI/Metal/MetalCpp"
)

if(NOT APPLE)
include_directories(${SpirvCross_INCLUDE_DIRS}
${Vulkan_INCLUDE_DIRS})
endif()

add_library(Renderer STATIC ${Srcs} ${Includes})
set_property(TARGET Renderer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Renderer Core Window msdfgen freetype)

target_link_libraries(Renderer
  Core
  Window
  ${msdfgen_LIBRARIES}
  ${freetype_LIBRARIES}
)

if(NOT APPLE)
  target_link_libraries(Renderer ${Vulkan_LIBRARIES} ${SpirvCross_LIBRARIES})
  #optimized "d3d12.lib" debug "d3d12.lib"
  #optimized "DXGI.lib" debug "DXGI.lib"
  
  option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" True)
  #set(RENDERER_ENABLE_VALIDATION_LAYER True)
  
  if (RENDERER_ENABLE_VALIDATION_LAYER)
	set(RENDERER_ENABLE_VALIDATION_LAYER_DEFINE "RENDERER_ENABLE_VALIDATION_LAYER")
  endif ()
else()
  target_link_libraries(Renderer "-framework Metal" ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES})
endif()

target_compile_definitions(Renderer PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE} ${RENDERER_ENABLE_VALIDATION_LAYER_DEFINE})
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Game_Debug>:${COMPILE_DEFINITIONS_GAME_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Game_Release>:${COMPILE_DEFINITIONS_GAME_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Editor_Debug>:${COMPILE_DEFINITIONS_EDITOR_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Editor_Release>:${COMPILE_DEFINITIONS_EDITOR_RELEASE}>" HOD_EXPORT)
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Server_Debug>:${COMPILE_DEFINITIONS_SERVER_DEBUG}>" HOD_EXPORT)
target_compile_definitions(Renderer PRIVATE "$<$<CONFIG:Server_Release>:${COMPILE_DEFINITIONS_SERVER_RELEASE}>" HOD_EXPORT)
