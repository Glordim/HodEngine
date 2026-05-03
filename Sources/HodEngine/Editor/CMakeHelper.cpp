#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/CMakeHelper.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Process/Process.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <fmt/format.h>
#include <fstream>
#include <string>

namespace hod::inline editor
{
	std::string_view CMakeHelper::GetCurrentPlatform()
	{
		return HOD_PLATFORM;
	}

	Path CMakeHelper::GetRunningToolchainPath()
	{
		return FileSystem::GetExecutablePath().ParentPath().ParentPath() / "cmake" / "toolchain.cmake";
	}

	Path CMakeHelper::GetRunningCmakeDirPath()
	{
		return FileSystem::GetExecutablePath().ParentPath().ParentPath().ParentPath().ParentPath().ParentPath() / "cmake";
	}

	Path CMakeHelper::GetInstalledCmakeDirPath()
	{
#ifdef HOD_INSTALL_PREFIX
		return Path(HOD_INSTALL_PREFIX) / "cmake";
#else
		// In a retail install the exe sits at Dist/platforms/<platform>/<type>/bin/,
		// so climbing 5 parent dirs lands on Dist/.
		return FileSystem::GetExecutablePath()
		    .ParentPath()  // exe filename
		    .ParentPath()  // bin/
		    .ParentPath()  // <type>/
		    .ParentPath()  // <platform>/
		    .ParentPath()  // platforms/
		    / "cmake";
#endif
	}

	Path CMakeHelper::GetInstalledToolchainPath()
	{
		Path toolchainPath;
#ifdef HOD_INSTALL_PREFIX
		toolchainPath = Path(HOD_INSTALL_PREFIX);
#else
		// In a retail install the exe sits at Dist/platforms/<platform>/<type>/bin/,
		// so climbing 5 parent dirs lands on Dist/.
		toolchainPath = FileSystem::GetExecutablePath()
		    .ParentPath()  // exe filename
		    .ParentPath()  // bin/
		    .ParentPath()  // <type>/
		    .ParentPath()  // <platform>/
		    .ParentPath()  // platforms/
#endif
		return toolchainPath / "platforms" / GetCurrentPlatform() / "static" / "cmake" / "toolchain.cmake";
	}

	bool CMakeHelper::GenerateCMakeLists(const Path& outputPath, std::string_view templateContent,
	                                     std::initializer_list<std::pair<std::string_view, std::string_view>> replacements)
	{
		std::string content(templateContent);
		for (const auto& [tag, value] : replacements)
		{
			size_t pos = 0;
			while ((pos = content.find(tag, pos)) != std::string::npos)
			{
				content.replace(pos, tag.size(), value);
				pos += value.size();
			}
		}

		try
		{
			std::ofstream file;
			file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
			file.open(outputPath.CStr(), std::ios_base::trunc);
			file.write(content.data(), content.size());
			file.close();
			OUTPUT_MESSAGE("CMakeLists.txt generated at {}", outputPath);
			return true;
		}
		catch (const std::ios_base::failure& e)
		{
			OUTPUT_ERROR("Failed to generate CMakeLists.txt at {} : {}", outputPath, e.what());
			return false;
		}
	}

	bool CMakeHelper::Configure(const Path& sourceDir, const Path& buildDir, const Path& installDir, Target target)
	{
		Path toolchainPath;
		Path engineDirPath;
		String gameBuildType;
		switch (target)
		{
			case Target::Retail:
				engineDirPath = GetInstalledCmakeDirPath();
				toolchainPath = GetInstalledToolchainPath();
				gameBuildType = "Application";
				break;

			case Target::Editor:
				engineDirPath = GetRunningCmakeDirPath();
				toolchainPath = GetRunningToolchainPath();
				gameBuildType = "GameModule";
				break;
		}

		String installDirArg;
		if (installDir.Empty() == false)
			installDirArg = fmt::format("-DCMAKE_INSTALL_PREFIX={}", installDir);

		std::string arguments = fmt::format("-DHOD_PLATFORM={} -DCMAKE_TOOLCHAIN_FILE:FILEPATH={} -DHodEngine_DIR={} -DHOD_GAME_BUILD_TYPE={} --no-warn-unused-cli -B {} -S {} {} -G Ninja",
		                                    GetCurrentPlatform(),
		                                    toolchainPath,
		                                    engineDirPath,
		                                    gameBuildType,
		                                    buildDir,
		                                    sourceDir,
		                                    installDirArg);
 
		OUTPUT_MESSAGE("Execute: cmake {}", arguments);
		return Process::Create("cmake", arguments, false);
	}

	bool CMakeHelper::Build(const Path& buildDir, const char* config)
	{
		std::string arguments = fmt::format("--build {} --config {} --parallel",
		                                    buildDir.GetString().CStr(),
		                                    config);

		OUTPUT_MESSAGE("Execute: cmake {}", arguments);
		return Process::Create("cmake", arguments, false);
	}

	bool CMakeHelper::Install(const Path& buildDir, const char* config)
	{
		std::string arguments = fmt::format("--install {} --config {}",
		                                    buildDir.GetString().CStr(),
		                                    config);

		OUTPUT_MESSAGE("Execute: cmake {}", arguments);
		return Process::Create("cmake", arguments, false);
	}
}
