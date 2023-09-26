#include <HodEngine/Core/ArgumentParser.h>
#include <HodEngine/Core/Output.h>
#include <HodEngine/Core/Stream/FileStream.h>

#include <filesystem>

#include "Lexer.h"

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

		std::filesystem::path headerOutputFilePath = outputFile;
		headerOutputFilePath += ".h";

		FileStream headerOutputStream(headerOutputFilePath, FileMode::Write);
		if (headerOutputStream.CanWrite() == false)
		{
			OUTPUT_ERROR("Unable to write output file : %s", headerOutputFilePath.string().c_str());
			return false;
		}

		headerOutputStream << "#pragma once\n\n";
		headerOutputStream << "#include <cstdint>\n\n";
		headerOutputStream << "namespace hod\n";
		headerOutputStream << "{\n";
		headerOutputStream << "\textern uint8_t " << outputFile.string().c_str() << "[];\n";
		headerOutputStream << "\textern uint32_t " << outputFile.string().c_str() << "_size;\n";
		headerOutputStream << "}\n";
		headerOutputStream.Close();

		std::filesystem::path sourceOutputFilePath = outputFile;
		sourceOutputFilePath += ".cpp";

		FileStream sourceOutputStream(sourceOutputFilePath, FileMode::Write);
		if (sourceOutputStream.CanWrite() == false)
		{
			OUTPUT_ERROR("Unable to write output file : %s", sourceOutputFilePath.string().c_str());
			return false;
		}

		sourceOutputStream << "#include <cstdint>\n\n";
		sourceOutputStream << "namespace hod\n";
		sourceOutputStream << "{\n";
		sourceOutputStream << "\tuint8_t " << outputFile.string().c_str() << "[] = {};\n";
		sourceOutputStream << "\tuint32_t " << outputFile.string().c_str() << "_size = sizeof(" << outputFile.string().c_str() << ");\n";
		sourceOutputStream << "}\n";
		sourceOutputStream.Close();

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
