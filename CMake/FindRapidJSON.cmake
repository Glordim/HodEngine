
if (NOT RapidJSON_FOUND)

if (NOT RapidJSON_ROOT_DIR)
    set (RapidJSON_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif (NOT RapidJSON_ROOT_DIR)

  find_path (RapidJSON_INCLUDE_DIRS
    NAMES rapidjson/rapidjson.h rapidjson/reader.h rapidjson/writer.h
    HINTS ${RapidJSON_ROOT_DIR} ${CMAKE_INSTALL_PREFIX}
    PATH_SUFFIXES include
  )

  message(STATUS "RapidJSON_INCLUDE_DIRS = ${RapidJSON_INCLUDE_DIRS}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(RapidJSON DEFAULT_MSG RapidJSON_INCLUDE_DIRS)

  mark_as_advanced(
    RapidJSON_INCLUDE_DIRS
  )

endif (NOT RapidJSON_FOUND)
