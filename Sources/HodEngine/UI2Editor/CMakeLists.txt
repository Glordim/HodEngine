project(UI2Editor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(UI2Editor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::UI2Editor ALIAS UI2Editor)
set_target_properties(UI2Editor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(UI2Editor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(UI2Editor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(UI2Editor
	PUBLIC UI2 Editor
)

target_compile_definitions(UI2Editor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_UI2_EDITOR_EXPORT
)

hod_install_lib(UI2Editor)
