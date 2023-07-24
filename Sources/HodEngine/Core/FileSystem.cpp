#include "HodEngine/Core/FileSystem.h"

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
#endif
}
