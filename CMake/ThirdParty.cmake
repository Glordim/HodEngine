set(THIRDPARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/install/${HOD_PLATFORM_NAME}/${HOD_ABI_NAME}")
if(NOT IS_DIRECTORY "${THIRDPARTY_ROOT}")
	message(FATAL_ERROR "ThirdParty not found: ${THIRDPARTY_ROOT}")
endif()
message(STATUS "ThirdParty: ${THIRDPARTY_ROOT}")

if(WIN32)
	set(LIB_PREFIX "")
	set(LIB_EXTENSION "lib")
	set(DLL_EXTENSION "dll")
	set(DLL_DIR "bin")
elseif(APPLE)
	set(LIB_PREFIX "lib")
	set(LIB_EXTENSION "a")
	set(DLL_EXTENSION "dylib")
	set(DLL_DIR "lib")
else()
	set(LIB_PREFIX "lib")
	set(LIB_EXTENSION "a")
	set(DLL_EXTENSION "so")
	set(DLL_DIR "lib")
endif()

find_package(fmt REQUIRED CONFIG
	PATHS "${THIRDPARTY_ROOT}/Fmt/lib/cmake"
	NO_DEFAULT_PATH
)

install(DIRECTORY "${THIRDPARTY_ROOT}/Fmt/lib/cmake/fmt"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
install(FILES "${THIRDPARTY_ROOT}/Fmt/lib/${LIB_PREFIX}fmt.${LIB_EXTENSION}"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib"
)
install(DIRECTORY "${THIRDPARTY_ROOT}/Fmt/include/fmt"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
)

# Copy fmt to build tree so HodEngineConfig.cmake works without install
file(COPY "${THIRDPARTY_ROOT}/Fmt/lib/cmake/fmt"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
file(COPY "${THIRDPARTY_ROOT}/Fmt/lib/${LIB_PREFIX}fmt.${LIB_EXTENSION}"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib"
)
file(COPY "${THIRDPARTY_ROOT}/Fmt/include/fmt"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
)

# Box2d
add_library(Box2d::Box2d STATIC IMPORTED GLOBAL)
set_target_properties(Box2d::Box2d PROPERTIES
	IMPORTED_LOCATION "${THIRDPARTY_ROOT}/Box2d/lib/${LIB_PREFIX}box2d.${LIB_EXTENSION}"
	INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/Box2d/include"
	INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/Box2d/include"
)

install(FILES "${THIRDPARTY_ROOT}/Box2d/lib/${LIB_PREFIX}box2d.${LIB_EXTENSION}"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib"
)
install(DIRECTORY "${THIRDPARTY_ROOT}/Box2d/include/"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
)
file(COPY "${THIRDPARTY_ROOT}/Box2d/lib/${LIB_PREFIX}box2d.${LIB_EXTENSION}"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib"
)
file(COPY "${THIRDPARTY_ROOT}/Box2d/include/"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
)

# Freetype
find_package(Freetype REQUIRED CONFIG
	PATHS "${THIRDPARTY_ROOT}/Freetype/lib/cmake"
	NO_DEFAULT_PATH
)

install(DIRECTORY "${THIRDPARTY_ROOT}/Freetype/lib/cmake/freetype"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
install(FILES "${THIRDPARTY_ROOT}/Freetype/lib/${LIB_PREFIX}freetype.${LIB_EXTENSION}"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib"
)
install(DIRECTORY "${THIRDPARTY_ROOT}/Freetype/include/"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
)
file(COPY "${THIRDPARTY_ROOT}/Freetype/lib/cmake/freetype"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
file(COPY "${THIRDPARTY_ROOT}/Freetype/lib/${LIB_PREFIX}freetype.${LIB_EXTENSION}"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib"
)
file(COPY "${THIRDPARTY_ROOT}/Freetype/include/"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
)

# EnkiTS
find_package(enkiTS REQUIRED CONFIG
	PATHS "${THIRDPARTY_ROOT}/EnkiTS/lib/cmake"
	NO_DEFAULT_PATH
)

install(DIRECTORY "${THIRDPARTY_ROOT}/EnkiTS/lib/cmake/enkiTS"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
install(FILES "${THIRDPARTY_ROOT}/EnkiTS/lib/${LIB_PREFIX}enkiTS.${LIB_EXTENSION}"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib"
)
install(DIRECTORY "${THIRDPARTY_ROOT}/EnkiTS/include/"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
)

# Copy enkiTS to build tree so HodEngineConfig.cmake works without install
file(COPY "${THIRDPARTY_ROOT}/EnkiTS/lib/cmake/enkiTS"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
)
file(COPY "${THIRDPARTY_ROOT}/EnkiTS/lib/${LIB_PREFIX}enkiTS.${LIB_EXTENSION}"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib"
)
file(COPY "${THIRDPARTY_ROOT}/EnkiTS/include/"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
)

# Stb (header-only)
add_library(Stb::Stb INTERFACE IMPORTED GLOBAL)
set_target_properties(Stb::Stb PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/Stb/include"
	INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/Stb/include"
)

install(DIRECTORY "${THIRDPARTY_ROOT}/Stb/include/"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
)
file(COPY "${THIRDPARTY_ROOT}/Stb/include/"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
)

# PortableFileDialogs (header-only)
add_library(PortableFileDialogs::PortableFileDialogs INTERFACE IMPORTED GLOBAL)
set_target_properties(PortableFileDialogs::PortableFileDialogs PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/PortableFileDialogs/include"
	INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/PortableFileDialogs/include"
)

install(FILES "${THIRDPARTY_ROOT}/PortableFileDialogs/include/portable-file-dialogs.h"
	DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
)
file(COPY "${THIRDPARTY_ROOT}/PortableFileDialogs/include/portable-file-dialogs.h"
	DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
)

# WindowsHModular (header-only, Windows only)
if(WIN32)
	add_library(WindowsHModular::WindowsHModular INTERFACE IMPORTED GLOBAL)
	set_target_properties(WindowsHModular::WindowsHModular PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/WindowsHModular/include"
		INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/WindowsHModular/include"
	)

	install(DIRECTORY "${THIRDPARTY_ROOT}/WindowsHModular/include/"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
	)
	file(COPY "${THIRDPARTY_ROOT}/WindowsHModular/include/"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
	)
endif()

if(NOT APPLE AND NOT ANDROID)
	find_package(VulkanHeaders REQUIRED CONFIG
		PATHS "${THIRDPARTY_ROOT}/VulkanHeaders/share/cmake"
		NO_DEFAULT_PATH
	)
	find_package(VulkanLoader REQUIRED CONFIG
		PATHS "${THIRDPARTY_ROOT}/VulkanLoader/lib/cmake"
		NO_DEFAULT_PATH
	)
	add_library(Vulkan::Vulkan INTERFACE IMPORTED GLOBAL)
	set_target_properties(Vulkan::Vulkan PROPERTIES
		INTERFACE_LINK_LIBRARIES "Vulkan::Loader;Vulkan::Headers"
	)

	install(DIRECTORY "${THIRDPARTY_ROOT}/VulkanHeaders/share/cmake/VulkanHeaders"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/share/cmake"
	)
	install(DIRECTORY "${THIRDPARTY_ROOT}/VulkanHeaders/include/"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
	)
	install(DIRECTORY "${THIRDPARTY_ROOT}/VulkanLoader/lib/cmake/VulkanLoader"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
	)
	install(FILES "${THIRDPARTY_ROOT}/VulkanLoader/lib/${LIB_PREFIX}vulkan-1.${LIB_EXTENSION}"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib"
	)
	install(FILES "${THIRDPARTY_ROOT}/VulkanLoader/bin/vulkan-1.${DLL_EXTENSION}"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/bin"
	)
	file(COPY "${THIRDPARTY_ROOT}/VulkanHeaders/share/cmake/VulkanHeaders"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/share/cmake"
	)
	file(COPY "${THIRDPARTY_ROOT}/VulkanHeaders/include/"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
	)
	file(COPY "${THIRDPARTY_ROOT}/VulkanLoader/lib/cmake/VulkanLoader"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib/cmake"
	)
	file(COPY "${THIRDPARTY_ROOT}/VulkanLoader/lib/${LIB_PREFIX}vulkan-1.${LIB_EXTENSION}"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib"
	)
	file(COPY "${THIRDPARTY_ROOT}/VulkanLoader/bin/vulkan-1.${DLL_EXTENSION}"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/bin"
	)
elseif(ANDROID)
	find_library(vulkan-lib vulkan)
	add_library(Vulkan::Vulkan SHARED IMPORTED)
	set_target_properties(Vulkan::Vulkan PROPERTIES
		IMPORTED_LOCATION "${vulkan-lib}"
		INTERFACE_INCLUDE_DIRECTORIES "${ANDROID_NDK}/toolchains/llvm/prebuilt/${ANDROID_HOST_TAG}/sysroot/usr/include"
	)
endif()

if(NOT APPLE)
	# VulkanMemoryAllocator
	find_package(VulkanMemoryAllocator REQUIRED CONFIG
		PATHS "${THIRDPARTY_ROOT}/VulkanMemoryAllocator/share/cmake"
		NO_DEFAULT_PATH
	)

	install(DIRECTORY "${THIRDPARTY_ROOT}/VulkanMemoryAllocator/share/cmake/VulkanMemoryAllocator"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/share/cmake"
	)
	install(FILES "${THIRDPARTY_ROOT}/VulkanMemoryAllocator/include/vk_mem_alloc.h"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
	)
	file(COPY "${THIRDPARTY_ROOT}/VulkanMemoryAllocator/share/cmake/VulkanMemoryAllocator"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/share/cmake"
	)
	file(COPY "${THIRDPARTY_ROOT}/VulkanMemoryAllocator/include/vk_mem_alloc.h"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
	)
else()
	add_library(MetalCpp::MetalCpp INTERFACE IMPORTED GLOBAL)
	set_target_properties(MetalCpp::MetalCpp PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/MetalCpp/include"
		INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THIRDPARTY_ROOT}/MetalCpp/include"
	)

	install(DIRECTORY "${THIRDPARTY_ROOT}/MetalCpp/include/"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
	)
	file(COPY "${THIRDPARTY_ROOT}/MetalCpp/include/"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
	)
endif()

if (RENDERER_ENABLE_VALIDATION_LAYER AND NOT APPLE)
	set(VULKAN_KHRONOS_VALIDATION_LAYER_LIB ${THIRDPARTY_ROOT}/VulkanValidationLayers/${DLL_DIR}/${LIB_PREFIX}VkLayer_khronos_validation.${DLL_EXTENSION})
	cmake_path(GET VULKAN_KHRONOS_VALIDATION_LAYER_LIB FILENAME VULKAN_KHRONOS_VALIDATION_LAYER_LIB_FILE_NAME)
	if (NOT ANDROID)
		set(VULKAN_KHRONOS_VALIDATION_LAYER_JSON ${THIRDPARTY_ROOT}/VulkanValidationLayers/${DLL_DIR}/${LIB_PREFIX}VkLayer_khronos_validation.json)
		cmake_path(GET VULKAN_KHRONOS_VALIDATION_LAYER_JSON FILENAME VULKAN_KHRONOS_VALIDATION_LAYER_JSON_FILE_NAME)
	endif()
endif()

if (ANDROID)
	# GameActivity
	set(GameActitvityDir "${CMAKE_SOURCE_DIR}/Externals/games-activity-4.4.2")

	add_library(AGDK::GameActitvity STATIC IMPORTED GLOBAL)
	set_target_properties(AGDK::GameActitvity PROPERTIES
		IMPORTED_LOCATION "${GameActitvityDir}/libs/android.${ANDROID_ABI}/libgame-activity_static.a"
		INTERFACE_INCLUDE_DIRECTORIES "${GameActitvityDir}/include"
		INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${GameActitvityDir}/include"
	)

	install(FILES "${GameActitvityDir}/libs/android.${ANDROID_ABI}/libgame-activity_static.a"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/lib"
	)
	install(DIRECTORY "${GameActitvityDir}/include/"
		DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/include"
	)
	install(FILES "${GameActitvityDir}/jar/games-activity.jar"
		DESTINATION "platforms/${HOD_PLATFORM_NAME}/jar"
	)
	file(COPY "${GameActitvityDir}/libs/android.${ANDROID_ABI}/libgame-activity_static.a"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/lib"
	)
	file(COPY "${GameActitvityDir}/include/"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/${HOD_PLATFORM_SUBDIR}/deps/include"
	)
	file(COPY "${GameActitvityDir}/jar/games-activity.jar"
		DESTINATION "${CMAKE_BINARY_DIR}/Output/platforms/${HOD_PLATFORM_NAME}/jar"
	)
endif()

find_package(benchmark REQUIRED CONFIG
	PATHS "${THIRDPARTY_ROOT}/Benchmark/lib/cmake"
	NO_DEFAULT_PATH
)

find_package(GTest REQUIRED CONFIG
	PATHS "${THIRDPARTY_ROOT}/Gtest/lib/cmake"
	NO_DEFAULT_PATH
)

find_package(xxHash REQUIRED CONFIG
	PATHS "${THIRDPARTY_ROOT}/xxHash/lib/cmake"
	NO_DEFAULT_PATH
)

if(ANDROID)
	# Cross-compilation: find slangc on the host (find_program bypasses the Android sysroot)
	set(_HOST_SLANG_BIN "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Install/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR}/Slang/bin")
	find_program(SLANG_COMPILER NAMES slangc slangc.exe
		HINTS "${_HOST_SLANG_BIN}"
		NO_DEFAULT_PATH
	)
	if(NOT SLANG_COMPILER)
		message(FATAL_ERROR "slangc not found in ${_HOST_SLANG_BIN}")
	endif()
	unset(_HOST_SLANG_BIN)
else()
	if (WIN32)
		find_package(slang REQUIRED CONFIG
			PATHS "${THIRDPARTY_ROOT}/Slang/cmake"
			NO_DEFAULT_PATH
		)
	else()
		find_package(slang REQUIRED CONFIG
			PATHS "${THIRDPARTY_ROOT}/Slang/lib/cmake"
			NO_DEFAULT_PATH
		)
	endif()
	set(SLANG_COMPILER "${SLANG_EXECUTABLE}")
endif()

unset(LIB_PREFIX)
unset(LIB_EXTENSION)
unset(DLL_EXTENSION)
unset(DLL_DIR)