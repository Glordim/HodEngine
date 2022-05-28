cmake_minimum_required(VERSION 3.4)

project(Application)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../CMake)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED) # imgui... todo remove

file(GLOB_RECURSE Srcs
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.inl"
)
assign_source_group(${Srcs})

add_library(Application ${Srcs})

if(MSVC)
	#target_compile_options(HodEngine-Application PRIVATE /W4 /WX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
else()
	#target_compile_options(HodEngine-Application PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_dependencies(Application Core)
get_target_property(Core_INCLUDES Core INCLUDE_DIRECTORIES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src
	".."
	${SDL2_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS} # imgui... todo remove
	${Core_INCLUDES}
)

target_link_libraries(Application
	Core
	${SDL2_LIBRARIES}
	${SDL2MAIN_LIBRARY}
	${Vulkan_LIBRARIES} # imgui... todo remove
)
