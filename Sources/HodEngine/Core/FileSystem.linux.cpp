#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <pwd.h>

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
			char executablePath[PATH_MAX] = { '\0' }; // init with 0 becausse readlink does not null terminate!
			if (readlink("/proc/self/exe", executablePath, PATH_MAX) == -1)
			{
				perror("readlink");
			}
			else
			{
				FileSystem::_executablePath = executablePath;
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
