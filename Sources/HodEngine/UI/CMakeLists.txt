project(UI LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UI ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UI ALIAS UI)
set_target_properties(UI PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(UI PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UI PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

# The base layer of the UI plugin (see HodPlugin.json): prefabs, nodes, layouts, canvases, usable without Game (the UIComponent and the
# runtime module are in UIComponents)
target_link_libraries(UI
	PUBLIC Core Math GameSystems Renderer
	PRIVATE Input
)

target_compile_definitions(UI PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(UI PUBLIC HOD_UI_STATIC)
else()
	target_compile_definitions(UI PRIVATE HOD_UI_EXPORT)
endif()

hod_install_lib(UI)
hod_plugin_manifest(UI UI "${CMAKE_CURRENT_SOURCE_DIR}/HodPlugin.json")
