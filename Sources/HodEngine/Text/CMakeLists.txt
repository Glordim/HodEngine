project(Text LANGUAGES CXX)

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes)

assign_source_group(${Srcs}) # for VS

add_library(Text ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::Text ALIAS Text)

set_target_properties(Text PROPERTIES
	FOLDER "HodEngine"
)
target_include_directories(Text PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(Text PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(Text
	PUBLIC HodEngine::Core HodEngine::Math
	PRIVATE HarfBuzz::HarfBuzz
)

target_compile_definitions(Text
	PUBLIC ${HOD_PLATFORM_DEFINE}
)

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(Text PUBLIC HOD_TEXT_STATIC)
else()
	target_compile_definitions(Text PRIVATE HOD_TEXT_EXPORT)
endif()

hod_install_lib(Text)
