cmake_minimum_required(VERSION 3.4...3.27)

function(FindHodEngine)
  if (DEFINED HodEngine_DIR)
    find_path(HodEngine_INCLUDE_DIRS NAMES HodEngine/HodEngine.hpp HINTS ${HodEngine_DIR}/Sources)          
    find_library(HodEngine_LIBRARIES NAMES HodEngine.lib HINTS ${HodEngine_DIR}/build/Editor_Debug PATH_SUFFIXES lib bin)

    message(STATUS "HodEngine_INCLUDE_DIRS = ${HodEngine_INCLUDE_DIRS}")
    message(STATUS "HodEngine_LIBRARIES = ${HodEngine_LIBRARIES}")
  else()
    message(FATAL_ERROR "HodEngine_DIR is not defined")
  endif()
endfunction()
