cmake_minimum_required(VERSION 3.10)

include(Global)
include(EmbedInSource)
include(EmbedShader)
include(ClangBuildAnalyzer)

if(APPLE)
	project(Renderer LANGUAGES CXX OBJCXX)
else()
	project(Renderer LANGUAGES CXX)
endif()

set(ExcludeDirs D3d12 Metal Vulkan)
if(NOT APPLE)
	if(WIN32)
		#set(API D3D12)
		#set(RENDERER_DEFINE "RENDERER_D3D12")
		set(API Vulkan)
		set(RENDERER_DEFINE "RENDERER_VULKAN")
	else()
		set(API Vulkan)
		set(RENDERER_DEFINE "RENDERER_VULKAN")
	endif()
else()
	set(API Metal)
	set(RENDERER_DEFINE "RENDERER_METAL")
endif()
list(REMOVE_ITEM ExcludeDirs ${API})

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES ${ExcludeDirs})

file(GLOB_RECURSE ShaderSources "${PROJECT_SOURCE_DIR}/Shader/*.slang")

set(GENERATED_DIR ${PROJECT_BINARY_DIR}/Generated)
set(SHADER_TMP_DIR ${PROJECT_BINARY_DIR}/ShaderTmp)
set(SHADER_OUTPUT_DIR ${GENERATED_DIR}/Shader)
set(SHADER_CPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.cpp.in)
set(SHADER_HPP_TEMPLATE ${PROJECT_SOURCE_DIR}/EmbedInSourceTemplate/ShaderSourceFileTemplate.hpp.in)
set(SHADER_CONFIG Debug)

set(GENERATED_SHADER_CPPS "")
set(GENERATED_SHADER_HPPS "")
EmbedShaders(ShaderSources ${API} ${SHADER_CONFIG} ${SHADER_CPP_TEMPLATE} ${SHADER_HPP_TEMPLATE} ${SHADER_OUTPUT_DIR} ${SHADER_TMP_DIR} GENERATED_SHADER_CPPS GENERATED_SHADER_HPPS)
list(APPEND Srcs ${GENERATED_SHADER_CPPS})
list(APPEND Includes ${GENERATED_SHADER_HPPS})

assign_source_group(${Srcs})
set_source_files_properties(RHI/Metal/MetalCpp.cpp StbImplementation.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) #Build SingleFileHeader lib with Implementation define separatly

add_library(Renderer ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Renderer ALIAS Renderer)
set_target_properties(Renderer PROPERTIES
	FOLDER "HodEngine"
	MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

target_include_directories(Renderer PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
	$<INSTALL_INTERFACE:include>
	PRIVATE ${GENERATED_DIR}
)
target_precompile_headers(Renderer PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Renderer
	PUBLIC Core Window
	PRIVATE Freetype::Freetype Stb::Stb
)

if(API STREQUAL Vulkan)
	if (NOT ANDROID)
		find_package(Vulkan REQUIRED)
	else()
		find_library(vulkan-lib vulkan)
		find_library(zlib-lib z)
		set(Vulkan_INCLUDE_DIRS ${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include) # TODO
		set(Vulkan_LIBRARIES ${vulkan-lib} ${zlib-lib}) # TODO
		find_package(BZip2 REQUIRED)
		find_package(ZLIB REQUIRED)
	endif()
	target_link_libraries(Renderer PRIVATE Vulkan::Vulkan VulkanMemoryAllocator::VulkanMemoryAllocator)
elseif(API STREQUAL Metal)
	find_library(QUARTZCORE_LIBRARY QuartzCore)
	find_library(METAL_LIBRARY Metal)
	target_link_libraries(Renderer PRIVATE ${QUARTZCORE_LIBRARY} ${METAL_LIBRARY} ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES})
elseif(API STREQUAL D3D12)
	target_link_libraries(Renderer PRIVATE "d3d12.lib" "DXGI.lib")
	target_compile_options(Renderer PRIVATE /clang:-Wno-language-extension-token)
endif()

unset(RENDERER_ENABLE_VALIDATION_LAYER CACHE)
option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" OFF)
if (RENDERER_ENABLE_VALIDATION_LAYER)
	target_compile_definitions(Renderer PRIVATE RENDERER_ENABLE_VALIDATION_LAYER)
endif()

target_compile_definitions(Renderer PRIVATE ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
target_compile_definitions(Renderer PRIVATE HOD_RENDERER_EXPORT)

install(TARGETS Renderer
	EXPORT HodEngineTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

ClangBuildAnalyzer(Renderer)
