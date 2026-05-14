get_filename_component(_toolchaineName ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
message(STATUS "Using HodEngine ${_toolchaineName} toolchain")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER "clang" CACHE STRING "Clang C compiler")
set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "Clang++ C++ compiler")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
