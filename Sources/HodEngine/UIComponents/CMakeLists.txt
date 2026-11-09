project(UIComponents LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UIComponents ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UIComponents ALIAS UIComponents)
set_target_properties(UIComponents PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(UIComponents PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UIComponents PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

# The Components layer of the UI module (see HodModule.json in UI): the only one that knows Game (the UIComponent, the runtime module's
# entry point), over the UI library that doesn't
target_link_libraries(UIComponents
	PUBLIC UI Renderer Game RendererComponents
)

target_compile_definitions(UIComponents PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(UIComponents PUBLIC HOD_UI_COMPONENTS_STATIC)
else()
	target_compile_definitions(UIComponents PRIVATE HOD_UI_COMPONENTS_EXPORT)
endif()

hod_install_lib(UIComponents)
