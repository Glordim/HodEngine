
if (NOT LibDecor_FOUND)

  find_path(LibDecor_INCLUDE_DIRS
    NAMES libdecor.h
    HINTS /usr/include/libdecor-0/
  )
    
  find_library(LibDecor_LIBRARIES NAMES libdecor-0.so
    HINTS /usr/lib/x86_64-linux-gnu
  )

  message(STATUS "LibDecor_INCLUDE_DIRS = ${LibDecor_INCLUDE_DIRS}")
  message(STATUS "LibDecor_LIBRARIES = ${LibDecor_LIBRARIES}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(LibDecor DEFAULT_MSG LibDecor_LIBRARIES LibDecor_INCLUDE_DIRS)

  mark_as_advanced(
    LibDecor_LIBRARIES
    LibDecor_INCLUDE_DIRS
  )

endif (NOT LibDecor_FOUND)
