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

find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(NOT APPLE)
    CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES D3d12 Metal)
else()
    CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES ".*/RHI/Vulkan/.*")
endif()

function(CompileReflectEmbedShader INPUT_SLANG TARGET_API SHADER_TYPE SHADER_CONFIG TEMPLATE_CPP TEMPLATE_HPP OUTPUT_CPP OUTPUT_HPP TEMP_DIR)
	add_custom_command(
		OUTPUT ${OUTPUT_CPP} ${OUTPUT_HPP}
		COMMAND ${Python3_EXECUTABLE}
			${CMAKE_SOURCE_DIR}/CMake/CompileReflectEmbedShader.py
			${SLANG_COMPILER}
			${INPUT_SLANG}
			${TARGET_API}
			${SHADER_TYPE}
			${SHADER_CONFIG}
			${TEMPLATE_CPP}
			${TEMPLATE_HPP}
			${OUTPUT_CPP}
			${OUTPUT_HPP}
			${TEMP_DIR}
		DEPENDS
			${INPUT_SLANG}
			${CMAKE_SOURCE_DIR}/CMake/CompileReflectEmbedShader.py
			${TEMPLATE_CPP}
			${TEMPLATE_HPP}
		COMMENT "Compiling ${SHADER_TYPE} shader from ${INPUT_SLANG}"
	)
endfunction()

set(ShaderCpps "")
set(ShaderHpps "")

file(GLOB_RECURSE ShaderSources "${PROJECT_SOURCE_DIR}/Shader/*.slang")

if(NOT APPLE)
	set(SHADER_TARGET_API Vulkan)
else()
	set(SHADER_TARGET_API Metal)
endif()
set(SHADER_TMP_DIR ${PROJECT_BINARY_DIR}/ShaderTmp)
set(SHADER_CPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.cpp.in)
set(SHADER_HPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/BinarySourceFileTemplate.hpp.in)

set(SHADER_CONFIG Debug)

foreach(file ${ShaderSources})
	get_filename_component(BASENAME "${file}" NAME_WE)

	set(SHADER_TYPE "Vertex")
	set(SHADER_EMBED_CPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.cpp)
	set(SHADER_EMBED_HPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.hpp)
	CompileReflectEmbedShader(${file} ${SHADER_TARGET_API} ${SHADER_TYPE} ${SHADER_CONFIG} ${SHADER_CPP_TEMPLATE} ${SHADER_HPP_TEMPLATE} ${SHADER_EMBED_CPP} ${SHADER_EMBED_HPP} ${SHADER_TMP_DIR})
	list(APPEND ShaderCpps ${SHADER_EMBED_CPP})
	list(APPEND ShaderHpps ${SHADER_EMBED_HPP})

	set(SHADER_TYPE "Fragment")
	set(SHADER_EMBED_CPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.cpp)
	set(SHADER_EMBED_HPP ${PROJECT_BINARY_DIR}/Shader/${BASENAME}_${SHADER_TYPE}.hpp)
	CompileReflectEmbedShader(${file} ${SHADER_TARGET_API} ${SHADER_TYPE} ${SHADER_CONFIG} ${SHADER_CPP_TEMPLATE} ${SHADER_HPP_TEMPLATE} ${SHADER_EMBED_CPP} ${SHADER_EMBED_HPP} ${SHADER_TMP_DIR})
	list(APPEND ShaderCpps ${SHADER_EMBED_CPP})
	list(APPEND ShaderHpps ${SHADER_EMBED_HPP})
endforeach()

list(APPEND Srcs ${ShaderCpps})
list(APPEND Includes ${ShaderHpps})

set_source_files_properties(RHI/Metal/MetalCpp.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) #Build SingleFileHeader lib with Implementation define separatly

assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    "../.."
    "${PROJECT_BINARY_DIR}/../.."
    ${PROJECT_BINARY_DIR}
    "RHI/Metal/MetalCpp"
)

add_library(Renderer ${LIB_TYPE} ${Srcs} ${Includes})
target_precompile_headers(Renderer PRIVATE "Pch.hpp")
set_property(TARGET Renderer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_dependencies(Renderer Core Window)

target_link_libraries(Renderer
    Core
    Window
    Freetype::Freetype
	Stb::Stb
	VulkanMemoryAllocator::VulkanMemoryAllocator
)


if(NOT APPLE)
  include_directories(${Vulkan_INCLUDE_DIRS} SpirvCross::SpirvCross)
  target_link_libraries(Renderer ${Vulkan_LIBRARIES} SpirvCross::SpirvCross)
  #optimized "d3d12.lib" debug "d3d12.lib"
  #optimized "DXGI.lib" debug "DXGI.lib"
  
  unset(RENDERER_ENABLE_VALIDATION_LAYER CACHE)
  option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" OFF)
  set(RENDERER_ENABLE_VALIDATION_LAYER True)
  
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
