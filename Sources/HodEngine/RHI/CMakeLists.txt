if(APPLE)
	project(RHI LANGUAGES CXX OBJCXX)
else()
	project(RHI LANGUAGES CXX)
endif()

set(ExcludeDirs D3d12 Metal Vulkan)
if(NOT APPLE)
	if(WIN32)
		#set(API D3D12)
		#set(RHI_DEFINE "RHI_D3D12")
		set(API Vulkan)
		set(RHI_DEFINE "RHI_VULKAN")
	else()
		set(API Vulkan)
		set(RHI_DEFINE "RHI_VULKAN")
	endif()
else()
	set(API Metal)
	set(RHI_DEFINE "RHI_METAL")
endif()
list(REMOVE_ITEM ExcludeDirs ${API})

CollectSourceFiles(${PROJECT_SOURCE_DIR} Srcs Includes EXCLUDES ${ExcludeDirs})

assign_source_group(${Srcs})
set_source_files_properties(Metal/MetalCpp.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) #Build SingleFileHeader lib with Implementation define separatly

add_library(RHI ${LIB_TYPE} ${Srcs} ${Includes})
add_library(HodEngine::RHI ALIAS RHI)
set_target_properties(RHI PROPERTIES
	FOLDER "HodEngine"
	HOD_RHI_BACKEND ${API} # read by the modules embedding shaders for the selected backend
)

target_include_directories(RHI PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Sources>
	$<INSTALL_INTERFACE:include>
)
target_precompile_headers(RHI PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_SOURCE_DIR}/Pch.hpp">)

target_link_libraries(RHI
	PUBLIC Core Math Window
)

if(API STREQUAL Vulkan)
	target_link_libraries(RHI PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)
	target_compile_definitions(RHI PRIVATE VMA_VULKAN_VERSION=1001000)
elseif(API STREQUAL Metal)
	find_library(QUARTZCORE_LIBRARY QuartzCore)
	find_library(METAL_LIBRARY Metal)
	target_link_libraries(RHI PRIVATE MetalCpp::MetalCpp ${QUARTZCORE_LIBRARY} ${METAL_LIBRARY})
elseif(API STREQUAL D3D12)
	target_link_libraries(RHI PRIVATE "d3d12.lib" "DXGI.lib")
	target_compile_options(RHI PRIVATE /clang:-Wno-language-extension-token)
endif()

if(APPLE)
	# Each .mm file auto-links libobjc via an embedded linker directive;
	# with several .mm files in this target, Xcode's linker sees "-lobjc"
	# more than once and warns. CMP0156 doesn't cover this (it only
	# de-duplicates libraries CMake itself puts on the link line).
	target_link_options(RHI PRIVATE "-Wl,-no_warn_duplicate_libraries")
endif()

if (RHI_ENABLE_VALIDATION_LAYER)
	target_compile_definitions(RHI PRIVATE RHI_ENABLE_VALIDATION_LAYER)
	if(API STREQUAL Vulkan)
		cmake_path(GET VULKAN_KHRONOS_VALIDATION_LAYER_LIB FILENAME VULKAN_KHRONOS_VALIDATION_LAYER_LIB_FILE_NAME)
		add_custom_command(
			TARGET RHI POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${VULKAN_KHRONOS_VALIDATION_LAYER_LIB}
				$<TARGET_FILE_DIR:RHI>/ValidationLayers/${VULKAN_KHRONOS_VALIDATION_LAYER_LIB_FILE_NAME}
			COMMENT "Copying validation layer (lib) to output directory"
		)
		if (ANDROID)
			install(FILES ${VULKAN_KHRONOS_VALIDATION_LAYER_LIB}
				DESTINATION "platforms/${HOD_PLATFORM_NAME}/jniLibs/${HOD_ABI_NAME}"
			)
		else()
			install(FILES ${VULKAN_KHRONOS_VALIDATION_LAYER_LIB}
				DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/ValidationLayers"
			)
		endif()
		if (NOT ANDROID)
			cmake_path(GET VULKAN_KHRONOS_VALIDATION_LAYER_JSON FILENAME VULKAN_KHRONOS_VALIDATION_LAYER_JSON_FILE_NAME)
			add_custom_command(
				TARGET RHI POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
					${VULKAN_KHRONOS_VALIDATION_LAYER_JSON}
					$<TARGET_FILE_DIR:RHI>/ValidationLayers/${VULKAN_KHRONOS_VALIDATION_LAYER_JSON_FILE_NAME}
				COMMENT "Copying validation layer (json) to output directory"
			)
			install(FILES ${VULKAN_KHRONOS_VALIDATION_LAYER_JSON}
				DESTINATION "${HOD_PLATFORM_SUBDIR}/deps/ValidationLayers"
			)
		endif()
	endif()
endif()

target_compile_definitions(RHI PUBLIC ${HOD_PLATFORM_DEFINE} PRIVATE ${RHI_DEFINE})
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(RHI PUBLIC HOD_RHI_STATIC)
else()
	target_compile_definitions(RHI PRIVATE HOD_RHI_EXPORT)
endif()

hod_install_lib(RHI)
