#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Output.hpp"

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(PLATFORM_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace hod
{
	std::filesystem::path FileSystem::_userSettingsPath;
	std::filesystem::path FileSystem::_executablePath;

#if defined(PLATFORM_WINDOWS)
	std::filesystem::path FileSystem::GetUserSettingsPath()
	{
		if (FileSystem::_userSettingsPath.empty() == true)
		{
			char username[256];
			unsigned long usernameLen = 256;
			GetUserNameA(username, &usernameLen);
			FileSystem::_userSettingsPath = std::string("C:\\users\\") + username + "\\AppData\\Local";
		}
		return FileSystem::_userSettingsPath;
	}

	std::filesystem::path FileSystem::GetExecutablePath()
	{
		if (FileSystem::_executablePath.empty() == true)
		{
			char executablePath[MAX_PATH];
			if (GetModuleFileNameA(nullptr, executablePath, MAX_PATH) != 0)
			{
				FileSystem::_executablePath = executablePath;
			}
		}
		return FileSystem::_executablePath;
	}

	bool FileSystem::SetWorkingDirectory(const std::filesystem::path& path)
	{
		if (SetCurrentDirectoryA(path.string().c_str()) == FALSE)
		{
			OUTPUT_ERROR("Unable to set working directory");
			return false;
		}
		return true;
	}

#elif defined(PLATFORM_LINUX)
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
			char executablePath[PATH_MAX];
			memset(executablePath, 0, sizeof(executablePath)); // readlink does not null terminate!
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
#endif
}
