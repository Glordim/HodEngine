#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <mach-o/dyld.h>
#include <sys/param.h>

namespace hod
{
	std::filesystem::path FileSystem::_userSettingsPath;
	std::filesystem::path FileSystem::_executablePath;

	std::filesystem::path FileSystem::GetUserSettingsPath()
	{
		if (FileSystem::_userSettingsPath.empty() == true)
		{
			struct passwd *pw = getpwuid(getuid());
			FileSystem::_userSettingsPath = pw->pw_dir;
		}
		return FileSystem::_userSettingsPath;
	}

	std::filesystem::path FileSystem::GetExecutablePath()
	{
		if (FileSystem::_executablePath.empty() == true)
		{
			char buffer[MAXPATHLEN];
			uint32_t bufferSize = sizeof(buffer);
			if (_NSGetExecutablePath(buffer, &bufferSize) == 0)
			{
				FileSystem::_executablePath = buffer;
			}
		}
		return FileSystem::_executablePath;
	}

	bool FileSystem::SetWorkingDirectory(const std::filesystem::path& path)
	{
		if (chdir(path.string().c_str()) != 0)
		{
			OUTPUT_ERROR("Unable to set working directory");
			return false;
		}
		return true;
	}
}
