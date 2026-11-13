project(PhysicsComponents LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(PhysicsComponents ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::PhysicsComponents ALIAS PhysicsComponents)
set_target_properties(PhysicsComponents PROPERTIES
	FOLDER "HodEngine"
)

target_include_directories(PhysicsComponents PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(PhysicsComponents PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

# The Components layer of the Physics plugin (see HodPlugin.json in Physics): the only one that knows Game (components, world system, the
# runtime module's entry point), over the Physics library that doesn't
target_link_libraries(PhysicsComponents
	PUBLIC Core Math Game Physics
)

target_compile_definitions(PhysicsComponents PUBLIC ${HOD_PLATFORM_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(PhysicsComponents PUBLIC HOD_PHYSICS_COMPONENTS_STATIC)
else()
	target_compile_definitions(PhysicsComponents PRIVATE HOD_PHYSICS_COMPONENTS_EXPORT)
endif()

hod_install_lib(PhysicsComponents)
