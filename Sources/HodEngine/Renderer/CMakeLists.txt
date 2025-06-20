cmake_minimum_required(VERSION 3.10)

include(Global)
include(EmbedInSource)

if(NOT APPLE)
project(Renderer LANGUAGES CXX)
if (NOT ANDROID)
  find_package(Vulkan REQUIRED)
else()
  find_library(vulkan-lib vulkan)
  find_library(zlib-lib z)
  set(Vulkan_INCLUDE_DIRS ${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include) # TODO
  set(Vulkan_LIBRARIES ${vulkan-lib} ${zlib-lib}) # TODO
endif()
else()
project(Renderer LANGUAGES CXX OBJCXX)
find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
endif()

if(NOT APPLE)
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES D3d12 Metal)
else()
CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES ".*/RHI/Vulkan/.*")
endif()

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

set_source_files_properties(RHI/Metal/MetalCpp.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) #Build SingleFileHeader lib with Implementation define separatly

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
"../.."
"${PROJECT_BINARY_DIR}/../.."
${PROJECT_BINARY_DIR}
"RHI/Metal/MetalCpp"
)

include_directories(SYSTEM
  Stb::Stb
  Freetype::Freetype
  VulkanMemoryAllocator::VulkanMemoryAllocator
)

add_library(Renderer ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(Renderer PRIVATE "Pch.hpp")
set_property(TARGET Renderer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Renderer Core Window)

target_link_libraries(Renderer
  Core
  Window
  Freetype::Freetype
)

if(NOT APPLE)
  include_directories(${Vulkan_INCLUDE_DIRS} SpirvCross::SpirvCross  Glslang::Glslang Glslang::GlslangDefaultResource)
  target_link_libraries(Renderer ${Vulkan_LIBRARIES} SpirvCross::SpirvCross Glslang::Glslang Glslang::GlslangDefaultResource)
  #optimized "d3d12.lib" debug "d3d12.lib"
  #optimized "DXGI.lib" debug "DXGI.lib"
  
  unset(RENDERER_ENABLE_VALIDATION_LAYER CACHE)
  option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" OFF)
  #set(RENDERER_ENABLE_VALIDATION_LAYER True)
  
  if (RENDERER_ENABLE_VALIDATION_LAYER)
    target_compile_definitions(Renderer PUBLIC RENDERER_ENABLE_VALIDATION_LAYER)
  endif ()
else()
  find_library(QUARTZCORE_LIBRARY QuartzCore)
  find_library(METAL_LIBRARY Metal)
  target_link_libraries(Renderer ${QUARTZCORE_LIBRARY} ${METAL_LIBRARY} ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES})
endif()

target_compile_definitions(Renderer PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(Renderer PRIVATE HOD_RENDERER_EXPORT)
