cmake_minimum_required(VERSION 3.4)

project(Core)

find_package(Glm REQUIRED)
find_package(RapidJSON REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE "../.."
	PUBLIC ${Glm_INCLUDE_DIRS}
	PUBLIC ${RapidJSON_INCLUDE_DIRS}
)

if(MSVC)
	#target_compile_options(HodEngine-Application PRIVATE /W4 /WX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
else()
	#target_compile_options(HodEngine-Application PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_library(Core OBJECT ${Srcs})

set (Uuid_LIB)

if (UNIX)
	set (Uuid_LIB uuid)
else ()
	set (Uuid_LIB "")
endif (UNIX)

target_link_libraries(
	Core
	${Glm_LIBRARIES}
	${Uuid_LIB}
)

target_compile_definitions(Core PUBLIC ${PLATFORM_DEFINE} ${RENDERER_DEFINE} ${RENDERER_ENABLE_VALIDATION_LAYER_DEFINE})
