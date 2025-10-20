#include <HodEngine/Core/FileSystem/Path.hpp>

#include "ArgumentParser.hpp"

#include "TokenReader.hpp"
#include "TokenWritter.hpp"

#include "Converter.hpp"
#include "ConverterGLSL.hpp"
#include "ConverterHLSL.hpp"
#include "ConverterMetal.hpp"

#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Target.hpp"

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h> // TODO create Processus class in Core lib
#elif defined(PLATFORM_MACOS)
	#include <spawn.h>
	#include <unistd.h>
#elif defined(PLATFORM_LINUX)
	#include <string.h> // strerror
	#include <sys/wait.h>
	#include <unistd.h>
#endif

namespace hod
{
	/// @brief
	/// @param inputFile
	/// @param outputFile
	/// @return
	bool ConvertShader(const Path& inputFile, const Path& outputFile, Target target)
	{
		std::cout << std::format("Convert '{}'...\n", inputFile.string());

		std::ifstream inputStream(inputFile);
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

		Path convertedOutputFilePath = outputFile;

		if (target == Target::Vulkan)
		{
			convertedOutputFilePath += ".glsl";
			ConverterGLSL converter;
			if (converter.Convert(tokens, convertedTokens) == false)
			{
				return false;
			}
		}
		else if (target == Target::Metal_MacOS || target == Target::Metal_IOS)
		{
			convertedOutputFilePath += ".metal";
			ConverterMetal converter;
			if (converter.Convert(tokens, convertedTokens) == false)
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		std::ofstream convertedOutputStream(convertedOutputFilePath);
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
	bool CompileShader(const Path& inputFile, const Path& outputFile, Target target)
	{
		Path finalInputFile = inputFile;

		if (target == Target::Vulkan)
		{
			finalInputFile += ".glsl";
		}
		else if (target == Target::Metal_MacOS || target == Target::Metal_IOS)
		{
			finalInputFile += ".metal";
		}
		else
		{
			return false;
		}

		bool result = false;

		// TODO create Processus class in Core lib
#if defined(PLATFORM_WINDOWS)
		STARTUPINFO         si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		std::string program = "%VULKAN_SDK%/Bin/glslangValidator.exe";

		CHAR resolvedProgram[MAX_PATH];
		ExpandEnvironmentStrings(program.c_str(), resolvedProgram, MAX_PATH);

		std::string commandLine = resolvedProgram;
		commandLine += " -Od --target-env vulkan1.3 -o ";
		commandLine += finalInputFile.string();
		commandLine += ".spirv ";
		commandLine += finalInputFile.string();

		// Start the child process.
		if (!CreateProcess(NULL,                       // No module name (use command line)
		                   (char*)commandLine.c_str(), // Command line
		                   NULL,                       // Process handle not inheritable
		                   NULL,                       // Thread handle not inheritable
		                   FALSE,                      // Set handle inheritance to FALSE
		                   0,                          // No creation flags
		                   NULL,                       // Use parent's environment block
		                   NULL,                       // Use parent's starting directory
		                   &si,                        // Pointer to STARTUPINFO structure
		                   &pi)                        // Pointer to PROCESS_INFORMATION structure
		)
		{
			std::cerr << std::format("CreateProcess failed ({})\n", GetLastError());
			return false;
		}

		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		result = true;
#elif defined(PLATFORM_MACOS)
		// Définition des arguments du programme à exécuter
		std::string inputFileStr = finalInputFile.string();
		std::string irFileStr = inputFileStr + ".ir";

		{
			const char* args[] = {"/usr/bin/xcrun",     "-sdk", target == Target::Metal_MacOS ? "macosx" : "iphoneos", "metal", "-o", irFileStr.c_str(), "-c",
			                      inputFileStr.c_str(), nullptr};

			int         argIndex = 0;
			const char* arg = args[argIndex];
			while (arg)
			{
				std::cout << arg << " ";

				++argIndex;
				arg = args[argIndex];
			}
			std::cout << std::endl;

			// Initialiser les attributs de spawn
			posix_spawnattr_t attr;
			posix_spawnattr_init(&attr);

			// Démarrer le processus fils
			pid_t pid;
			int   status = posix_spawn(&pid, args[0], nullptr, &attr, (char* const*)args, NULL);
			if (status == 0)
			{
				std::cout << "Processus lancé avec succès. PID : " << pid << std::endl;
				// Attendre que le processus fils se termine
				waitpid(pid, &status, 0);
				std::cout << "Le processus fils s'est terminé avec le statut : " << status << std::endl;
				result = true;
			}
			else
			{
				std::cerr << "Erreur lors du lancement du processus : " << strerror(status) << std::endl;
			}

			// Libérer les attributs de spawn
			posix_spawnattr_destroy(&attr);
		}

		std::string libFileStr = inputFileStr + ".metallib";

		{
			const char* args[] = {"/usr/bin/xcrun",   "-sdk", target == Target::Metal_MacOS ? "macosx" : "iphoneos", "metallib", irFileStr.c_str(), "-o",
			                      libFileStr.c_str(), nullptr};

			int         argIndex = 0;
			const char* arg = args[argIndex];
			while (arg)
			{
				std::cout << arg << " ";

				++argIndex;
				arg = args[argIndex];
			}
			std::cout << std::endl;

			// Initialiser les attributs de spawn
			posix_spawnattr_t attr;
			posix_spawnattr_init(&attr);

			// Démarrer le processus fils
			pid_t pid;
			int   status = posix_spawn(&pid, args[0], nullptr, &attr, (char* const*)args, NULL);
			if (status == 0)
			{
				std::cout << "Processus lancé avec succès. PID : " << pid << std::endl;
				// Attendre que le processus fils se termine
				waitpid(pid, &status, 0);
				std::cout << "Le processus fils s'est terminé avec le statut : " << status << std::endl;
				result = true;
			}
			else
			{
				std::cerr << "Erreur lors du lancement du processus : " << strerror(status) << std::endl;
			}

			// Libérer les attributs de spawn
			posix_spawnattr_destroy(&attr);
		}
#elif defined(PLATFORM_LINUX)
		pid_t pid = fork(); // Crée un nouveau processus
		if (pid == -1)
		{
			std::cerr << std::format("CreateProcess failed ({})\n", strerror(errno));
			return false;
		}

		if (pid == 0) // Child
		{
			std::string strOutputFile = finalInputFile.string() + ".spirv";
			std::string strInputFile = finalInputFile.string();
			const char* argv[] = {"glslangValidator", "-Od", "--target-env", "vulkan1.3", "-o", strOutputFile.c_str(), strInputFile.c_str(), nullptr};

			execvp(argv[0], (char* const*)argv);
			std::cerr << "Exec failed" << std::endl;
			return 1;
		}
		else // Parent
		{
			int status;
			waitpid(pid, &status, 0);

			if (WIFEXITED(status))
			{
				std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
				result = true;
			}
			else
			{
				std::cerr << "Child did not exit normally" << std::endl;
				result = false;
			}
		}
#else
	#error
#endif
		return result;
	}

	/// @brief
	/// @param inputFile
	/// @param outputFile
	/// @return
	bool EmbeedInSource(const Path& inputFile, const Path& outputFile, Target target)
	{
		Path finalInputFile = inputFile;

		if (target == Target::Vulkan)
		{
			finalInputFile += ".glsl.spirv";
		}
		else if (target == Target::Metal_MacOS || target == Target::Metal_IOS)
		{
			finalInputFile += ".metal.metallib";
		}
		else
		{
			return false;
		}

		std::ifstream inputStream(finalInputFile, std::ios::binary);
		if (inputStream.is_open() == false)
		{
			std::cerr << std::format("Unable to read output file : {}\n", finalInputFile.string());
			return false;
		}

		Path headerOutputFilePath = outputFile;
		headerOutputFilePath += ".hpp";

		std::ofstream headerOutputStream(headerOutputFilePath);
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
		// headerOutputStream.Close();

		Path sourceOutputFilePath = outputFile;
		sourceOutputFilePath += ".cpp";

		std::ofstream sourceOutputStream(sourceOutputFilePath);
		if (sourceOutputStream.is_open() == false)
		{
			std::cerr << std::format("Unable to write output file : {}\n", headerOutputFilePath.string());
			return false;
		}

		sourceOutputStream << "#include <cstdint>\n\n";
		sourceOutputStream << "namespace hod\n";
		sourceOutputStream << "{\n";
		sourceOutputStream << "\tuint8_t " << identifier << "[] = {\n\t\t";

		int  count = 0;
		char value;
		while (inputStream.get(value))
		{
			// std::stringstream stream;
			// stream << "0x";
			// stream << std::setfill ('0') << std::setw(sizeof(value)*2);
			// stream << std::hex << i;
			// stream << std::hex << static_cast<uint8_t>(value);//std::to_string(static_cast<uint8_t>(value));
			// sourceOutputStream << stream.str();
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
		// sourceOutputStream.Close();

		return true;
	}

	/// @brief
	/// @param argc
	/// @param argv
	/// @return
	bool Run(int argc, char** argv)
	{
		ArgumentParser argumentParser(argc, argv);

		Path inputDirectory = std::filesystem::current_path();

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

		Path outputDirectory = std::filesystem::current_path();

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

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
		Target target = Target::Vulkan;
#elif defined(PLATFORM_MACOS)
		Target target = Target::Metal_MacOS;
#else
	#error
#endif
		const hod::Argument* targetArgument = argumentParser.GetArgument('t', "target");
		if (targetArgument != nullptr)
		{
			if (targetArgument->_valueCount == 0)
			{
				std::cerr << "--target argument is empty, please insert a valid path\n";
				return false;
			}
			else if (targetArgument->_valueCount > 1)
			{
				std::cerr << "--target argument support only one path\n";
				return false;
			}

			static std::array<std::pair<std::string_view, Target>, (uint8_t)Target::Count> targetEnumLabels = {
				std::pair<std::string_view, Target>("Vulkan", Target::Vulkan),
				std::pair<std::string_view, Target>("Metal_MacOS", Target::Metal_MacOS),
				std::pair<std::string_view, Target>("Metal_IOS", Target::Metal_IOS),
				std::pair<std::string_view, Target>("D3D12_Windows", Target::D3D12_Windows),
			};

			for (const std::pair<std::string_view, Target>& targetEnumLabel : targetEnumLabels)
			{
				if (targetEnumLabel.first == outputArgument->_values[0])
				{
					target = targetEnumLabel.second;
					break;
				}
			}

			if (target == Target::Count)
			{
				std::cerr << "--target incalid value\n";
				return false;
			}
		}

#if defined(PLATFORM_WINDOWS)
		if (target != Target::Vulkan && target != Target::D3D12_Windows)
#elif defined(PLATFORM_MACOS)
		if (target != Target::Metal_MacOS && target != Target::Metal_IOS)
#elif defined(PLATFORM_LINUX)
		if (target != Target::Vulkan)
#else
	#error
#endif
		{
			std::cerr << "Target unavailable on  this platform\n";
			return false;
		}

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

			if (ConvertShader(entry.path(), outputDirectory / entry.path().filename(), target) == false)
			{
				return false;
			}

			if (CompileShader(outputDirectory / entry.path().filename(), outputDirectory / entry.path().filename(), target) == false)
			{
				return false;
			}

			if (EmbeedInSource(outputDirectory / entry.path().filename(), outputDirectory / entry.path().filename(), target) == false)
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
