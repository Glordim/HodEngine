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

function(EmbeedShaderSource InputFile TemplateFile OutputDir OutputFile)
  message(STATUS "Pack Shader: ${InputFile}")

  get_filename_component(FILENAME "${InputFile}" NAME)
  string(REPLACE "." "_" ESCAPED_FILE_NAME "${FILENAME}")

  file(READ "${InputFile}" FILE_CONTENTS)
  string(REPLACE "\"" "\\\"" ESCAPED_FILE_CONTENTS "${FILE_CONTENTS}")
  string(REPLACE "\n" "\\n\"\n\"" ESCAPED_FILE_CONTENTS "${ESCAPED_FILE_CONTENTS}")

  configure_file(
      ${TemplateFile}
      ${OutputDir}/${FILENAME}.hpp
      @ONLY
  )
endfunction()

file(GLOB_RECURSE ShaderSources
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.vert"
	"${PROJECT_SOURCE_DIR}/Shader/Sources/*.frag"
)

foreach(file ${ShaderSources})
  EmbeedShaderSource(${file} ${PROJECT_SOURCE_DIR}/Shader/Template/ShaderSourceFileTemplate.hpp.in ${PROJECT_SOURCE_DIR}/Shader/Generated GeneratedFile)
  list(APPEND Srcs ${GeneratedFile})
  list(APPEND Includes ${GeneratedFile})
endforeach()

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
"../.."
${Stb_INCLUDE_DIRS}
${msdfgen_INCLUDE_DIRS}
${freetype_INCLUDE_DIRS}
${VulkanMemoryAllocator_INCLUDE_DIRS}
"RHI/Metal/MetalCpp"
)

add_library(Renderer ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(Renderer PRIVATE "Pch.hpp")
set_property(TARGET Renderer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Renderer Core Window msdfgen freetype VulkanMemoryAllocator)

target_link_libraries(Renderer
  Core
  Window
  ${msdfgen_LIBRARIES}
  ${freetype_LIBRARIES}
  ${VulkanMemoryAllocator_LIBRARIES}
)

if(NOT APPLE)
  add_dependencies(Renderer glslang SpirvCross)
  include_directories(${SpirvCross_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS} ${glslang_INCLUDE_DIRS})
  target_link_libraries(Renderer ${Vulkan_LIBRARIES} ${SpirvCross_LIBRARIES} ${glslang_LIBRARIES})
  #optimized "d3d12.lib" debug "d3d12.lib"
  #optimized "DXGI.lib" debug "DXGI.lib"
  
  unset(RENDERER_ENABLE_VALIDATION_LAYER CACHE)
  option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" OFF)
  #set(RENDERER_ENABLE_VALIDATION_LAYER True)
  
  if (RENDERER_ENABLE_VALIDATION_LAYER)
    target_compile_definitions(Renderer PUBLIC RENDERER_ENABLE_VALIDATION_LAYER)
  endif ()
else()
  target_link_libraries(Renderer "-framework Metal" ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES})
endif()

target_compile_definitions(Renderer PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(Renderer PRIVATE HOD_RENDERER_EXPORT)
