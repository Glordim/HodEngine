get_filename_component(_toolchaineName ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
message(STATUS "Using HodEngine ${_toolchaineName} toolchain")

if(NOT DEFINED ANDROID_NDK)
	if(DEFINED ENV{ANDROID_NDK_ROOT})
		set(ANDROID_NDK "$ENV{ANDROID_NDK_ROOT}")
	elseif(DEFINED ENV{ANDROID_NDK_HOME})
		set(ANDROID_NDK "$ENV{ANDROID_NDK_HOME}")
	elseif(DEFINED ENV{ANDROID_NDK})
		set(ANDROID_NDK "$ENV{ANDROID_NDK}")
	else()
		message(FATAL_ERROR "ANDROID_NDK_ROOT, ANDROID_NDK_HOME or ANDROID_NDK environment variable must be set")
	endif()
endif()

set(ANDROID_ABI "x86_64" CACHE STRING "Android ABI")
set(ANDROID_PLATFORM "android-29" CACHE STRING "Android API level")

include("${ANDROID_NDK}/build/cmake/android.toolchain.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
