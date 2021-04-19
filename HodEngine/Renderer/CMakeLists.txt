# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

project(Renderer)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)

assign_source_group(${Srcs})

include_directories("./src"
".."
"${SDK_DIR}/glm"
"${SDK_DIR}/stb"
"${SDK_DIR}/SPIRV-Cross"
"${SDK_DIR}/rapidjson/include"
)

add_library(Renderer ${Srcs})

add_dependencies(Renderer Core DebugLayer Application)

target_link_libraries(Renderer
Core
DebugLayer
Application
optimized "${SDK_DIR}/Build/SPIRV-Cross/Release/spirv-cross-core.lib" debug "${SDK_DIR}/Build/SPIRV-Cross/Debug/spirv-cross-core.lib"
optimized "${SDK_DIR}/Build/glm/glm/Release/glm_static.lib" debug "${SDK_DIR}/Build/glm/glm/Debug/glm_static.lib"
optimized "d3d12.lib" debug "d3d12.lib"
optimized "DXGI.lib" debug "DXGI.lib"
)

find_package(Vulkan)
if (Vulkan_FOUND)
  # Une fois la dépendance trouvée, nous l'incluons au projet
  target_include_directories(Renderer PUBLIC ${Vulkan_INCLUDE_DIRS})
  target_link_libraries (Renderer ${Vulkan_LIBRARIES})
else ()
  # Sinon, nous affichons un message
  message(FATAL_ERROR "Vulkan not found")
endif ()
