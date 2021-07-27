
if (NOT RapidJSON_FOUND)

  find_path (RapidJSON_INCLUDE_DIRS
    NAMES rapidjson/rapidjson.h rapidjson/reader.h rapidjson/writer.h
    HINTS ${SDK_DIR}/rapidjson/include
  )

  message(STATUS "RapidJSON_INCLUDE_DIRS = ${RapidJSON_INCLUDE_DIRS}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(RapidJSON DEFAULT_MSG RapidJSON_INCLUDE_DIRS)

  mark_as_advanced(
    RapidJSON_INCLUDE_DIRS
  )

endif (NOT RapidJSON_FOUND)
