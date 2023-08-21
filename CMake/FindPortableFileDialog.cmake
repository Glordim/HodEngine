
if (NOT PortableFileDialog_FOUND)

  find_path (PortableFileDialog_INCLUDE_DIRS
    NAMES portable-file-dialogs.h
    HINTS ${portable_file_dialogs_SOURCE_DIR}
  )

  message(STATUS "PortableFileDialog_INCLUDE_DIRS = ${PortableFileDialog_INCLUDE_DIRS}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(PortableFileDialog DEFAULT_MSG Stb_INCLUDE_DIRS)

  mark_as_advanced(
    PortableFileDialog_INCLUDE_DIRS
  )

endif (NOT PortableFileDialog_FOUND)
