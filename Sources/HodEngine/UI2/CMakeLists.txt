project(UI2 LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UI2 ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UI2 ALIAS UI2)
set_target_properties(UI2 PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(UI2 PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UI2 PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(UI2
	PUBLIC Renderer
)

target_compile_definitions(UI2 PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(UI2 PUBLIC HOD_UI2_STATIC)
else()
	target_compile_definitions(UI2 PRIVATE HOD_UI2_EXPORT)
endif()

hod_install_lib(UI2)
