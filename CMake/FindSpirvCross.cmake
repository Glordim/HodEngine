
if (NOT SpirvCross_FOUND)

  find_path(SpirvCross_INCLUDE_DIRS
    NAMES SPIRV-Cross/spirv_cross.hpp
    HINTS ${SDK_DIR}
  )
  
  find_library(SpirvCross_LIBRARIES NAMES spirv-cross-core libspirv-cross-core
    HINTS ${SDK_DIR}/Build/SPIRV-Cross
  )

  message(STATUS "SpirvCross_INCLUDE_DIRS = ${SpirvCross_INCLUDE_DIRS}")
  message(STATUS "SpirvCross_LIBRARIES = ${SpirvCross_LIBRARIES}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(SpirvCross DEFAULT_MSG SpirvCross_LIBRARIES SpirvCross_INCLUDE_DIRS)

  mark_as_advanced(
    SpirvCross_LIBRARIES
    SpirvCross_INCLUDE_DIRS
  )

endif (NOT SpirvCross_FOUND)
