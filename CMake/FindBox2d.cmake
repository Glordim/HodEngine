
if (NOT Box2d_FOUND)

	set(VC_LIB_PATH_SUFFIX bin/Release)

	find_path(Box2d_INCLUDE_DIRS
		NAMES box2d/box2d.h
		HINTS ${SDK_DIR}/box2d/include
	)

	find_library(Box2d_LIBRARIES NAMES box2d libbox2d
		HINTS ${SDK_DIR}/Build/box2d
		PATH_SUFFIXES lib bin ${VC_LIB_PATH_SUFFIX}
	)

	message(STATUS "Box2d_INCLUDE_DIRS = ${Box2d_INCLUDE_DIRS}")
	message(STATUS "Box2d_LIBRARIES = ${Box2d_LIBRARIES}")

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Box2d DEFAULT_MSG Box2d_LIBRARIES Box2d_INCLUDE_DIRS)

	mark_as_advanced(
		Box2d_LIBRARIES
		Box2d_INCLUDE_DIRS
	)

endif (NOT Box2d_FOUND)
