cmake_minimum_required(VERSION 3.4...3.27)

include(Global)

if(NOT APPLE)
project(Renderer LANGUAGES CXX)
if (NOT ANDROID)
  find_package(Vulkan REQUIRED)
else()
  set(Vulkan_INCLUDE_DIRS ${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include) # TODO
  set(Vulkan_LIBRARIES ${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/x86_64-linux-android/33/libvulkan.so ${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/x86_64-linux-android/libz.a) # TODO
endif()
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

foreach(file ${ShaderSources})
  EmbeedTextInSource(${file} ${PROJECT_SOURCE_DIR}/Shader/Template/ShaderSourceFileTemplate.hpp.in ${PROJECT_SOURCE_DIR}/Shader/Generated GeneratedFile)
  list(APPEND Srcs ${GeneratedFile})
  list(APPEND Includes ${GeneratedFile})
endforeach()

assign_source_group(${Srcs}) # for VS

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
"../.."
"RHI/Metal/MetalCpp"
)

include_directories(SYSTEM
  ${Stb_INCLUDE_DIRS}
  ${msdfgen_INCLUDE_DIRS}
  ${freetype_INCLUDE_DIRS}
  ${VulkanMemoryAllocator_INCLUDE_DIRS}
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
