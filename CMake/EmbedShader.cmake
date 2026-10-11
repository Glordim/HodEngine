# slangc emits DXIL through dxcompiler.dll and signs it with dxil.dll, which both come with the Windows SDK.
# Sets HOD_DXC_DIR to the directory holding them.
function(FindDxc)
	if(HOD_DXC_DIR AND EXISTS "${HOD_DXC_DIR}/dxcompiler.dll")
		return()
	endif()
	unset(HOD_DXC_DIR CACHE)

	if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
		set(_arch arm64)
	else()
		set(_arch x64)
	endif()

	set(_hints)
	if(DEFINED ENV{WindowsSdkVerBinPath})
		file(TO_CMAKE_PATH "$ENV{WindowsSdkVerBinPath}" _sdkVerBin)
		list(APPEND _hints "${_sdkVerBin}/${_arch}")
	endif()
	foreach(_programFiles "$ENV{ProgramFiles\(x86\)}" "$ENV{ProgramFiles}")
		if(_programFiles)
			file(TO_CMAKE_PATH "${_programFiles}" _programFiles)
			file(GLOB _sdkBins "${_programFiles}/Windows Kits/10/bin/10.*")
			list(SORT _sdkBins COMPARE NATURAL ORDER DESCENDING) # newest first
			foreach(_sdkBin ${_sdkBins})
				list(APPEND _hints "${_sdkBin}/${_arch}")
			endforeach()
		endif()
	endforeach()

	find_path(HOD_DXC_DIR dxcompiler.dll HINTS ${_hints} ENV PATH DOC "Directory of dxcompiler.dll and dxil.dll")
	if(NOT HOD_DXC_DIR)
		message(FATAL_ERROR "dxcompiler.dll not found: the D3D12 shaders need the DirectX Shader Compiler of the Windows SDK (or set HOD_DXC_DIR)")
	endif()
endfunction()

function(CompileReflectEmbedShader INPUT_SLANG TARGET_API SHADER_TYPE SHADER_CONFIG TEMPLATE_CPP TEMPLATE_HPP OUTPUT_CPP OUTPUT_HPP TEMP_DIR)
	find_package(Python3 REQUIRED COMPONENTS Interpreter)

	# What the shaders import, like the attributes the engine reads from their reflection
	set(SHADER_LIB_DIR "${CMAKE_SOURCE_DIR}/Sources/HodEngine/Renderer/ShaderLib")
	file(GLOB_RECURSE ShaderLibSources "${SHADER_LIB_DIR}/*.slang")

	set(DXC_DIR_ARG "")
	string(TOLOWER "${TARGET_API}" _targetApi)
	if(_targetApi STREQUAL "d3d12")
		FindDxc()
		set(DXC_DIR_ARG "${HOD_DXC_DIR}")
	endif()

	add_custom_command(
		OUTPUT ${OUTPUT_CPP} ${OUTPUT_HPP}
		COMMAND ${Python3_EXECUTABLE}
			${CMAKE_SOURCE_DIR}/CMake/CompileReflectEmbedShader.py
			${SLANG_COMPILER}
			${INPUT_SLANG}
			${TARGET_API}
			${SHADER_TYPE}
			${SHADER_CONFIG}
			${TEMPLATE_CPP}
			${TEMPLATE_HPP}
			${OUTPUT_CPP}
			${OUTPUT_HPP}
			${TEMP_DIR}
			${SHADER_LIB_DIR}
			${DXC_DIR_ARG}
		DEPENDS
			${INPUT_SLANG}
			${ShaderLibSources}
			${CMAKE_SOURCE_DIR}/CMake/CompileReflectEmbedShader.py
			${TEMPLATE_CPP}
			${TEMPLATE_HPP}
		COMMENT "Compiling ${SHADER_TYPE} shader from ${INPUT_SLANG}"
	)
endfunction()

function(EmbedShaders SHADER_SOURCES TARGET_API SHADER_CONFIG TEMPLATE_CPP TEMPLATE_HPP OUTPUT_DIR TEMP_DIR OUT_CPPS OUT_HPPS)
	set(ShaderCpps "")
	set(ShaderHpps "")
	file(MAKE_DIRECTORY ${TEMP_DIR})
	file(MAKE_DIRECTORY ${OUTPUT_DIR})
	foreach(file IN LISTS ${SHADER_SOURCES})
		get_filename_component(BASENAME "${file}" NAME_WE)

		set(SHADER_TYPE "Vertex")
		set(SHADER_EMBED_CPP ${OUTPUT_DIR}/${BASENAME}_${SHADER_TYPE}.cpp)
		set(SHADER_EMBED_HPP ${OUTPUT_DIR}/${BASENAME}_${SHADER_TYPE}.hpp)
		CompileReflectEmbedShader(${file} ${TARGET_API} ${SHADER_TYPE} ${SHADER_CONFIG} ${TEMPLATE_CPP} ${TEMPLATE_HPP} ${SHADER_EMBED_CPP} ${SHADER_EMBED_HPP} ${TEMP_DIR})
		list(APPEND ShaderCpps ${SHADER_EMBED_CPP})
		list(APPEND ShaderHpps ${SHADER_EMBED_HPP})

		set(SHADER_TYPE "Fragment")
		set(SHADER_EMBED_CPP ${OUTPUT_DIR}/${BASENAME}_${SHADER_TYPE}.cpp)
		set(SHADER_EMBED_HPP ${OUTPUT_DIR}/${BASENAME}_${SHADER_TYPE}.hpp)
		CompileReflectEmbedShader(${file} ${TARGET_API} ${SHADER_TYPE} ${SHADER_CONFIG} ${TEMPLATE_CPP} ${TEMPLATE_HPP} ${SHADER_EMBED_CPP} ${SHADER_EMBED_HPP} ${TEMP_DIR})
		list(APPEND ShaderCpps ${SHADER_EMBED_CPP})
		list(APPEND ShaderHpps ${SHADER_EMBED_HPP})
	endforeach()
	set(${OUT_CPPS} ${ShaderCpps} PARENT_SCOPE)
	set(${OUT_HPPS} ${ShaderHpps} PARENT_SCOPE)
endfunction()
