
if (NOT Stb_FOUND)

  find_path (Stb_INCLUDE_DIRS
    NAMES stb/stb_image.h
    HINTS ${SDK_DIR}
  )

  message(STATUS "Stb_INCLUDE_DIRS = ${Stb_INCLUDE_DIRS}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Stb DEFAULT_MSG Stb_INCLUDE_DIRS)

  mark_as_advanced(
    Stb_INCLUDE_DIRS
  )

endif (NOT Stb_FOUND)
