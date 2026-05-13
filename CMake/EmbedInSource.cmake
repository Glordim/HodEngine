function(EmbedTextInSource InputFile TemplateFile OutputDir OutOutputFileHPP)

	get_filename_component(FILENAME "${InputFile}" NAME)
	set(OutputFileHPP "${OutputDir}/${FILENAME}.hpp")
	set(${OutOutputFileH} "${OutputFileHPP}" PARENT_SCOPE)

	message(STATUS "EmbedTextInSource")
	message(STATUS "Src: ${InputFile}")
	message(STATUS "Dst: ${OutputDir}/${FILENAME}.hpp")

	string(REGEX REPLACE "\\.| |-" "_" ESCAPED_FILE_NAME ${FILENAME})

	file(READ "${InputFile}" FILE_CONTENTS)
	string(REPLACE "\"" "\\\"" ESCAPED_FILE_CONTENTS "${FILE_CONTENTS}")
	string(REPLACE "\n" "\\n\"\n\"" ESCAPED_FILE_CONTENTS "${ESCAPED_FILE_CONTENTS}")

	configure_file(
		${TemplateFile}
		${OutputFileHPP}
		@ONLY
	)
endfunction()

function(EmbedBinaryInSource InputFile TemplateFileHPP TemplateFileCPP OutputDir OutOutputFileHPP OutOutputFileCPP)

	get_filename_component(FILENAME "${InputFile}" NAME)

	set(OutputFileHPP "${OutputDir}/${FILENAME}.hpp")
	set(OutputFileCPP "${OutputDir}/${FILENAME}.cpp")
	set(${OutOutputFileHPP} "${OutputFileHPP}" PARENT_SCOPE)
	set(${OutOutputFileCPP} "${OutputFileCPP}" PARENT_SCOPE)

	message(STATUS "EmbedBinaryInSource")
	message(STATUS "Src: ${InputFile}")
	message(STATUS "Dst Cpp: ${OutputFileCPP}")
	message(STATUS "Dst Hpp: ${OutputFileHPP}")

	string(REGEX REPLACE "\\.| |-" "_" ESCAPED_FILE_NAME ${FILENAME})

	file(READ "${InputFile}" FILE_CONTENTS HEX)
	string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," ESCAPED_FILE_CONTENTS ${FILE_CONTENTS}) # Convert hex data for C compatibility

	configure_file(
		${TemplateFileHPP}
		${OutputFileHPP}
		@ONLY
	)

	configure_file(
		${TemplateFileCPP}
		${OutputFileCPP}
		@ONLY
	)
endfunction()
