cmake_minimum_required(VERSION 3.4)

project(ImGui)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../CMake)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
  ".."
  ${SDL2_INCLUDE_DIRS}
  ${Vulkan_INCLUDE_DIRS}
)

add_library(ImGui ${Srcs})

target_link_libraries(ImGui
  ${SDL2_LIBRARIES}
  ${Vulkan_LIBRARIES}
)
