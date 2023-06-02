cmake_minimum_required(VERSION 3.4)

project(Renderer)

find_package(Vulkan REQUIRED)
find_package(SpirvCross REQUIRED)
find_package(Stb REQUIRED)
find_package(Glm REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
  "../.."
  ${Stb_INCLUDE_DIRS}
  ${SpirvCross_INCLUDE_DIRS}
  ${Vulkan_INCLUDE_DIRS}
  ${Glm_INCLUDE_DIRS}
)

if(MSVC)
	#target_compile_options(HodEngine-Application PRIVATE /W4 /WX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
else()
	#target_compile_options(HodEngine-Application PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_library(Renderer OBJECT ${Srcs})

add_dependencies(Renderer Core Window)

target_link_libraries(Renderer
  Core
  Window
  optimized "d3d12.lib" debug "d3d12.lib"
  optimized "DXGI.lib" debug "DXGI.lib"
  ${Vulkan_LIBRARIES}
  ${SpirvCross_LIBRARIES}
)

option(RENDERER_ENABLE_VALIDATION_LAYER "Enable ValidationLayer" True)
#set(RENDERER_ENABLE_VALIDATION_LAYER True)

if (RENDERER_ENABLE_VALIDATION_LAYER)
	set(RENDERER_ENABLE_VALIDATION_LAYER_DEFINE "RENDERER_ENABLE_VALIDATION_LAYER")
endif ()

target_compile_definitions(Renderer PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE} ${RENDERER_ENABLE_VALIDATION_LAYER_DEFINE})
