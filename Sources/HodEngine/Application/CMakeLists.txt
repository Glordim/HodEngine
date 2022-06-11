cmake_minimum_required(VERSION 3.4)

project(Application)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

add_library(Application OBJECT ${Srcs})

if(MSVC)
	#target_compile_options(HodEngine-Application PRIVATE /W4 /WX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
else()
	#target_compile_options(HodEngine-Application PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_dependencies(Application Core Window Renderer)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)
get_target_property(Window_INCLUDES Window INCLUDE_DIRECTORIES)
get_target_property(Renderer_INCLUDES Renderer INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
	"../.."
	${Core_INCLUDES}
	${Window_INCLUDES}
	${Renderer_INCLUDES}
)

target_link_libraries(Application
	Core
	Window
	Renderer
)

target_compile_definitions(Application PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE})
