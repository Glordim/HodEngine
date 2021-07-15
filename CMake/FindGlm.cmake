
if (NOT Glm_FOUND)

  find_path (Glm_INCLUDE_DIRS
    NAMES glm/glm.hpp
    HINTS ${SDK_DIR}/glm
  )

  message(STATUS "Glm_INCLUDE_DIRS = ${Glm_INCLUDE_DIRS}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Glm DEFAULT_MSG Glm_INCLUDE_DIRS)

  mark_as_advanced(
    Glm_INCLUDE_DIRS
  )

endif (NOT Glm_FOUND)
