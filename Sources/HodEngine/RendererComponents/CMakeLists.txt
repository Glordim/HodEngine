project(RendererComponents LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(RendererComponents ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::RendererComponents ALIAS RendererComponents)
set_target_properties(RendererComponents PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(RendererComponents PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(RendererComponents PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

# The Components layer of the Renderer plugin (see HodPlugin.json in Renderer): the only one that knows Game (its components, the world system drawing
# the worlds, the runtime module's entry point), over the Renderer library that doesn't
target_link_libraries(RendererComponents
	PUBLIC Core Math GameSystems Window Renderer Game
)

target_compile_definitions(RendererComponents PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(RendererComponents PUBLIC HOD_RENDERER_COMPONENTS_STATIC)
else()
	target_compile_definitions(RendererComponents PRIVATE HOD_RENDERER_COMPONENTS_EXPORT)
endif()

hod_install_lib(RendererComponents)
