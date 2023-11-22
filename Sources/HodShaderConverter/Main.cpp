#include <HodEngine/Core/ArgumentParser.h>
#include <HodEngine/Core/Output.h>
#include <HodEngine/Core/Stream/FileStream.h>

#include <filesystem>

#include "Lexer.h"
#include "Converter.h"
#include "ConverterGLSL.h"
#include "ConverterHLSL.h"

#include <fstream>

#include <Windows.h> // TODO create Processus class in Core lib

namespace hod
{
	/// @brief 
	/// @param inputFile 
	/// @param outputFile 
	/// @return 
	bool ConvertShader(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile)
	{
		OUTPUT_MESSAGE("Convert '%s'...", inputFile.string().c_str());

		FileStream inputStream(inputFile, FileMode::Read);
		if (inputStream.CanRead() == false)
		{
			OUTPUT_ERROR("Unable to read input file : %s", inputFile.string().c_str());
			return false;
		}

		std::vector<ShaderLangToken> tokens;

		Lexer lexer;
		if (lexer.Tokenize(inputStream, tokens) == false)
		{
			return false;
		}

		ConverterGLSL converter;
		if (converter.Convert(tokens) == false)
		{
			return false;
		}

		std::filesystem::path convertedOutputFilePath = outputFile;
		convertedOutputFilePath += ".glsl";

		std::ofstream convertedOutputStream(convertedOutputFilePath, 0);
		if (convertedOutputStream.is_open() == false)
		{
			OUTPUT_ERROR("Unable to write output file : %s", convertedOutputFilePath.string().c_str());
			return false;
		}

		convertedOutputStream << converter.GetResult();
		return true;
	}

	/// @brief 
	/// @param inputFile 
	/// @param outputFile 
	/// @return 
	bool CompileShader(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile)
	{
		std::filesystem::path finalInputFile = inputFile;
		finalInputFile += ".glsl";

		// TODO create Processus class in Core lib

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		std::string program = "%VULKAN_SDK%/Bin/glslangValidator.exe";
		std::string commandLine = program;
		/*
		commandLine += " -Od --target-env vulkan1.1 -o ";
		commandLine += finalInputFile.string();
		commandLine += ".spirv ";
		commandLine += finalInputFile.string();
		*/

		// Start the child process. 
		if( !CreateProcess( NULL,   // No module name (use command line)
			(char*)commandLine.c_str(),// Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
		{
			printf( "CreateProcess failed (%d).\n", GetLastError() );
			return false;
		}

		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return true;
	}

	/// @brief 
	/// @param inputFile 
	/// @param outputFile 
	/// @return 
	bool EmbeedInSource(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile)
	{
		FileStream inputStream(inputFile, FileMode::Read);
		if (inputStream.CanRead() == false)
		{
			OUTPUT_ERROR("Unable to read input file : %s", inputFile.string().c_str());
			return false;
		}

		// todo

		std::filesystem::path headerOutputFilePath = outputFile;
		headerOutputFilePath += ".h";

		std::ofstream headerOutputStream(headerOutputFilePath, 0);
		if (headerOutputStream.is_open() == false)
		{
			OUTPUT_ERROR("Unable to write output file : %s", headerOutputFilePath.string().c_str());
			return false;
		}
		/*FileStream headerOutputStream(headerOutputFilePath, FileMode::Write);
		if (headerOutputStream.CanWrite() == false)
		{
			OUTPUT_ERROR("Unable to write output file : %s", headerOutputFilePath.string().c_str());
			return false;
		}*/

		headerOutputStream << "#pragma once\n\n";
		headerOutputStream << "#include <cstdint>\n\n";
		headerOutputStream << "namespace hod\n";
		headerOutputStream << "{\n";
		headerOutputStream << "\textern uint8_t " << outputFile.stem().string() << "[];\n";
		headerOutputStream << "\textern uint32_t " << outputFile.stem().string() << "_size;\n";
		headerOutputStream << "}\n";
		//headerOutputStream.Close();

		std::filesystem::path sourceOutputFilePath = outputFile;
		sourceOutputFilePath += ".cpp";

		std::ofstream sourceOutputStream(sourceOutputFilePath, 0);
		if (sourceOutputStream.is_open() == false)
		{
			OUTPUT_ERROR("Unable to write output file : %s", headerOutputFilePath.string().c_str());
			return false;
		}
		/*
		FileStream sourceOutputStream(sourceOutputFilePath, FileMode::Write);
		if (sourceOutputStream.CanWrite() == false)
		{
			OUTPUT_ERROR("Unable to write output file : %s", sourceOutputFilePath.string().c_str());
			return false;
		}
		*/

		sourceOutputStream << "#include <cstdint>\n\n";
		sourceOutputStream << "namespace hod\n";
		sourceOutputStream << "{\n";
		sourceOutputStream << "\tuint8_t " << outputFile.stem().string() << "[] = \"" << /*converter.GetResult().c_str() <<*/ "\";\n";
		sourceOutputStream << "\tuint32_t " << outputFile.stem().string() << "_size = sizeof(" << outputFile.stem().string() << ");\n";
		sourceOutputStream << "}\n";
		//sourceOutputStream.Close();

		return true;
	}

	/// @brief 
	/// @param argc 
	/// @param argv 
	/// @return 
	bool Run(int argc, char** argv)
	{
		ArgumentParser argumentParser(argc, argv);
		
		std::filesystem::path inputDirectory = std::filesystem::current_path();

		const Argument* inputArgument = argumentParser.GetArgument('i', "input");
		if (inputArgument != nullptr)
		{
			if (inputArgument->_valueCount == 0)
			{
				OUTPUT_ERROR("--input argument is empty, please insert a valid path");
				return 1;
			}
			else if (inputArgument->_valueCount > 1)
			{
				OUTPUT_ERROR("--input argument support only one path");
				return 1;
			}
			inputDirectory = inputArgument->_values[0];
		}

		if (std::filesystem::exists(inputDirectory) == false)
		{
			OUTPUT_ERROR("Input directory doesn't exist : %s", inputDirectory.string().c_str());
			return 1;
		}
		OUTPUT_MESSAGE("Input '%s'", inputDirectory.string().c_str());

		std::filesystem::path outputDirectory = std::filesystem::current_path();

		const hod::Argument* outputArgument = argumentParser.GetArgument('o', "output");
		if (outputArgument != nullptr)
		{
			if (outputArgument->_valueCount == 0)
			{
				OUTPUT_ERROR("--output argument is empty, please insert a valid path");
				return 1;
			}
			else if (outputArgument->_valueCount > 1)
			{
				OUTPUT_ERROR("--output argument support only one path");
				return 1;
			}
			outputDirectory = outputArgument->_values[0];
		}

		if (std::filesystem::exists(outputDirectory) == false)
		{
			if (std::filesystem::create_directories(outputDirectory) == false)
			{
				OUTPUT_ERROR("Unable to create Output directory : %s", outputDirectory.string().c_str());
				return 1;
			}
		}
		OUTPUT_MESSAGE("Output '%s'", outputDirectory.string().c_str());

		for (const auto& entry : std::filesystem::directory_iterator(inputDirectory))
		{
			if (entry.is_directory() == true) // todo support recursivity ?
			{
				continue;
			}

			if (entry.is_regular_file() == false)
			{
				continue;
			}

			if (ConvertShader(entry.path(), outputDirectory / entry.path().stem()) == false)
			{
				return 1;
			}

			if (CompileShader(entry.path().stem(), entry.path().stem()) == false)
			{
				return 1;
			}
		}

		return 0;
	}
}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv)
{
	if (hod::Run(argc, argv) == false)
	{
		return 1;
	}

	return 0;
}
