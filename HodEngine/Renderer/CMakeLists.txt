cmake_minimum_required(VERSION 3.4)

project(Renderer)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../CMake)
find_package(Vulkan REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(SpirvCross REQUIRED)
find_package(Stb REQUIRED)
find_package(Glm REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

include_directories("./src"
  ".."
  ${Stb_INCLUDE_DIRS}
  ${SpirvCross_INCLUDE_DIRS}
  ${RapidJSON_INCLUDE_DIRS}
  ${Vulkan_INCLUDE_DIRS}
  ${Glm_INCLUDE_DIRS}
)

add_library(Renderer ${Srcs})

add_dependencies(Renderer Core DebugLayer Application)

target_link_libraries(Renderer
  Core
  DebugLayer
  Application
  #optimized "d3d12.lib" debug "d3d12.lib"
  #optimized "DXGI.lib" debug "DXGI.lib"
  ${Vulkan_LIBRARIES}
  ${SpirvCross_LIBRARIES}
)
