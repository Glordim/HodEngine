
if (NOT SDL2_FOUND)

	set(VC_LIB_PATH_SUFFIX Release)

	find_path(SDL2_INCLUDE_DIRS
		NAMES SDL.h
		HINTS ${SDK_DIR}/SDL/include
		PATH_SUFFIXES SDL2 include include/SDL2
	)

	find_library(SDL2_LIBRARIES NAMES SDL2 libSDL2
		HINTS ${SDK_DIR}/Build/SDL
		PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
	)

	message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
	message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_LIBRARIES SDL2_INCLUDE_DIRS)

	mark_as_advanced(
		SDL2_LIBRARIES
		SDL2_INCLUDE_DIRS
	)

endif (NOT SDL2_FOUND)
