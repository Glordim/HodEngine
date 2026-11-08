project(PhysicsEditor LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

add_library(PhysicsEditor ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::PhysicsEditor ALIAS PhysicsEditor)
set_target_properties(PhysicsEditor PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(PhysicsEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(PhysicsEditor PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(PhysicsEditor
	PUBLIC Physics GameEditor Editor
)

target_compile_definitions(PhysicsEditor
	PUBLIC ${HOD_PLATFORM_DEFINE}
	PRIVATE HOD_PHYSICS_EDITOR_EXPORT
)

hod_install_lib(PhysicsEditor)
