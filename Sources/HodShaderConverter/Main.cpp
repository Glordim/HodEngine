#include <filesystem>

#include "ArgumentParser.hpp"

#include "TokenReader.hpp"
#include "TokenWritter.hpp"

#include "Converter.hpp"
#include "ConverterGLSL.hpp"
#include "ConverterHLSL.hpp"

#include <fstream>
#include <iomanip>
#include <format>
#include <iostream>

#include <Windows.h> // TODO create Processus class in Core lib

namespace hod
{
	/// @brief 
	/// @param inputFile 
	/// @param outputFile 
	/// @return 
	bool ConvertShader(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile)
	{
		std::cout << std::format("Convert '{}'...\n", inputFile.string());

		std::ifstream inputStream(inputFile, 0);
		if (inputStream.is_open() == false)
		{
			std::cerr << std::format("Unable to read input file : {}\n", inputFile.string());
			return false;
		}

		std::vector<Token> tokens;

		TokenReader tokenReader;
		if (tokenReader.Parse(inputStream, tokens) == false)
		{
			return false;
		}

		std::vector<Token> convertedTokens;
		convertedTokens.reserve(tokens.size());

		ConverterGLSL converter;
		if (converter.Convert(tokens, convertedTokens) == false)
		{
			return false;
		}

		std::filesystem::path convertedOutputFilePath = outputFile;
		convertedOutputFilePath += ".glsl";

		std::ofstream convertedOutputStream(convertedOutputFilePath, 0);
		if (convertedOutputStream.is_open() == false)
		{
			std::cerr << std::format("Unable to write output file : {}\n", convertedOutputFilePath.string());
			return false;
		}

		TokenWritter tokenWritter;
		if (tokenWritter.PrettyWrite(convertedOutputStream, convertedTokens) == false)
		{
			return false;
		}

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

		CHAR resolvedProgram[MAX_PATH];
		ExpandEnvironmentStrings(program.c_str(), resolvedProgram, MAX_PATH);

		std::string commandLine = resolvedProgram;
		commandLine += " -Od --target-env vulkan1.3 -o ";
		commandLine += finalInputFile.string();
		commandLine += ".spirv ";
		commandLine += finalInputFile.string();

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
			std::cerr << std::format("CreateProcess failed ({})\n", GetLastError());
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
		std::filesystem::path finalInputFile = inputFile;
		finalInputFile += ".glsl";
		finalInputFile += ".spirv";

		std::ifstream inputStream(finalInputFile, std::ios::binary);
		if (inputStream.is_open() == false)
		{
			std::cerr << std::format("Unable to read output file : {}\n", finalInputFile.string());
			return false;
		}

		std::filesystem::path headerOutputFilePath = outputFile;
		headerOutputFilePath += ".hpp";

		std::ofstream headerOutputStream(headerOutputFilePath, 0);
		if (headerOutputStream.is_open() == false)
		{
			std::cerr << std::format("Unable to write output file : {}\n", headerOutputFilePath.string());
			return false;
		}

		std::string identifier = outputFile.filename().string();
		std::replace(identifier.begin(), identifier.end(), '.', '_');

		headerOutputStream << "#pragma once\n\n";
		headerOutputStream << "#include <cstdint>\n\n";
		headerOutputStream << "namespace hod\n";
		headerOutputStream << "{\n";
		headerOutputStream << "\textern uint8_t " << identifier << "[];\n";
		headerOutputStream << "\textern uint32_t " << identifier << "_size;\n";
		headerOutputStream << "}\n";
		//headerOutputStream.Close();

		std::filesystem::path sourceOutputFilePath = outputFile;
		sourceOutputFilePath += ".cpp";

		std::ofstream sourceOutputStream(sourceOutputFilePath, 0);
		if (sourceOutputStream.is_open() == false)
		{
			std::cerr << std::format("Unable to write output file : {}\n", headerOutputFilePath.string());
			return false;
		}

		sourceOutputStream << "#include <cstdint>\n\n";
		sourceOutputStream << "namespace hod\n";
		sourceOutputStream << "{\n";
		sourceOutputStream << "\tuint8_t " << identifier << "[] = {\n\t\t";

		int count = 0;
		char value;
		while (inputStream.get(value))
		{
			//std::stringstream stream;
			//stream << "0x";
         	//stream << std::setfill ('0') << std::setw(sizeof(value)*2);
         	//stream << std::hex << i;
			//stream << std::hex << static_cast<uint8_t>(value);//std::to_string(static_cast<uint8_t>(value));
			//sourceOutputStream << stream.str();
			sourceOutputStream << std::format("0x{:02X}", static_cast<uint8_t>(value));
			sourceOutputStream << ", ";
			++count;

			if (count > 32)
			{
				sourceOutputStream << "\n\t\t";
				count = 0;
			}
		}
		sourceOutputStream << "\n\t};\n";
		sourceOutputStream << "\tuint32_t " << identifier << "_size = sizeof(" << identifier << ");\n";
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
				std::cerr << "--input argument is empty, please insert a valid path\n";
				return false;
			}
			else if (inputArgument->_valueCount > 1)
			{
				std::cerr << "--input argument support only one path\n";
				return false;
			}
			inputDirectory = inputArgument->_values[0];
		}

		if (std::filesystem::exists(inputDirectory) == false)
		{
			std::cerr << std::format("Input directory doesn't exist : {}\n", inputDirectory.string());
			return false;
		}
		std::cout << std::format("Input '{}'\n", inputDirectory.string());

		std::filesystem::path outputDirectory = std::filesystem::current_path();

		const hod::Argument* outputArgument = argumentParser.GetArgument('o', "output");
		if (outputArgument != nullptr)
		{
			if (outputArgument->_valueCount == 0)
			{
				std::cerr << "--output argument is empty, please insert a valid path\n";
				return false;
			}
			else if (outputArgument->_valueCount > 1)
			{
				std::cerr << "--output argument support only one path\n";
				return false;
			}
			outputDirectory = outputArgument->_values[0];
		}

		if (std::filesystem::exists(outputDirectory) == false)
		{
			if (std::filesystem::create_directories(outputDirectory) == false)
			{
				std::cerr << std::format("Unable to create Output directory : {}\n", outputDirectory.string());
				return false;
			}
		}
		std::cout << std::format("Output '{}'\n", outputDirectory.string());

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

			if (ConvertShader(entry.path(), outputDirectory / entry.path().filename()) == false)
			{
				return false;
			}

			if (CompileShader(outputDirectory / entry.path().filename(), outputDirectory / entry.path().filename()) == false)
			{
				return false;
			}

			if (EmbeedInSource(outputDirectory / entry.path().filename(), outputDirectory / entry.path().filename()) == false)
			{
				return false;
			}
		}

		return true;
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
