cmake_minimum_required(VERSION 3.4)

project(Physics)

find_package(Box2d REQUIRED)
find_package(Glm REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
	"../.."
	${Box2d_INCLUDE_DIRS}
	${Glm_INCLUDE_DIRS}
)

if(MSVC)
	#target_compile_options(HodEngine-Application PRIVATE /W4 /WX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
else()
	#target_compile_options(HodEngine-Application PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_library(Physics OBJECT ${Srcs})

add_dependencies(Physics Core Renderer)

target_link_libraries(Physics
	Core
	Renderer
	${Box2d_LIBRARIES}
)
