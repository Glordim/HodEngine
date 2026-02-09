cmake_minimum_required(VERSION 3.10)

function(ClangBuildAnalyzer Target)
	if(ENABLE_CLANG_BUILD_ANALYZER)
		if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			if (MSVC)
				target_compile_options(${Target} PRIVATE "/clang:-ftime-trace")
			else()
				target_compile_options(${Target} PRIVATE "-ftime-trace")
			endif()
		else()
			message(WARNING "ENABLE_CLANG_BUILD_ANALYZER est ON mais le compilateur n'est pas Clang.")
		endif()

		set(ANALYSIS_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.buildAnalysis")

		add_custom_command(TARGET ${Target} POST_BUILD
			COMMAND ${CLANG_BUILD_ANALYZER_PATH} --all ${CMAKE_CURRENT_BINARY_DIR} ${ANALYSIS_OUTPUT}
			COMMAND ${CLANG_BUILD_ANALYZER_PATH} --analyze ${ANALYSIS_OUTPUT}
			COMMENT "Analyse du build avec ClangBuildAnalyzer pour ${PROJECT_NAME}..."
			VERBATIM
		)
	endif()
endfunction()
