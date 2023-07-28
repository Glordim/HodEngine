
if (NOT SpirvCross_FOUND)

  if(WIN32)
    set(SPIRVCross_DIR $ENV{VULKAN_SDK})
    message(STATUS "SPIRVCross_DIR = ${SPIRVCross_DIR}")
    
    find_path(SpirvCross_INCLUDE_DIRS
      NAMES spirv_cross/spirv_cross.hpp
      HINTS ${SPIRVCross_DIR}/Include
    )
      
    find_library(SpirvCross_LIBRARIES NAMES spirv-cross-core.lib
      HINTS ${SPIRVCross_DIR}/Lib
      PATH_SUFFIXES lib bin
    )

  elseif(UNIX)

    find_path(SpirvCross_INCLUDE_DIRS
      NAMES spirv_cross/spirv_cross.hpp
      HINTS /usr/include
    )
      
    find_library(SpirvCross_LIBRARIES NAMES libspirv-cross-core.a
      HINTS /usr/lib/x86_64-linux-gnu
    )

  endif()

  message(STATUS "SpirvCross_INCLUDE_DIRS = ${SpirvCross_INCLUDE_DIRS}")
  message(STATUS "SpirvCross_LIBRARIES = ${SpirvCross_LIBRARIES}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(SpirvCross DEFAULT_MSG SpirvCross_LIBRARIES SpirvCross_INCLUDE_DIRS)

  mark_as_advanced(
    SpirvCross_LIBRARIES
    SpirvCross_INCLUDE_DIRS
  )

endif (NOT SpirvCross_FOUND)
