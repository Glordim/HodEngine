cmake_minimum_required(VERSION 3.4)

project(Core)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../CMake)
find_package(Glm REQUIRED)

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/Src/*.cpp"
	"${PROJECT_SOURCE_DIR}/Src/*.h"
	"${PROJECT_SOURCE_DIR}/Src/*.inl"
)
assign_source_group(${Srcs})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
	PRIVATE ".."
	PUBLIC ${Glm_INCLUDE_DIRS}
)

add_library(Core ${Srcs})

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
