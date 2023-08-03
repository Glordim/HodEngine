
if (NOT XkbCommon_FOUND)

  find_path(XkbCommon_INCLUDE_DIRS
    NAMES xkbcommon/xkbcommon.h
    HINTS /usr/include/
  )
    
  find_library(XkbCommon_LIBRARIES NAMES libxkbcommon.so
    HINTS /usr/lib/x86_64-linux-gnu
  )

  message(STATUS "XkbCommon_INCLUDE_DIRS = ${XkbCommon_INCLUDE_DIRS}")
  message(STATUS "XkbCommon_LIBRARIES = ${XkbCommon_LIBRARIES}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(XkbCommon DEFAULT_MSG XkbCommon_LIBRARIES XkbCommon_INCLUDE_DIRS)

  mark_as_advanced(
    XkbCommon_LIBRARIES
    XkbCommon_INCLUDE_DIRS
  )

endif (NOT XkbCommon_FOUND)
