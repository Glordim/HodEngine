#include "HodEngine/Core/FileSystem.h"

#include <Windows.h>

namespace hod
{
	namespace core
	{
		std::filesystem::path FileSystem::_userSettingsPath = "C:\\%USER%\\AppData\\Local";

		std::filesystem::path FileSystem::GetUserSettingsPath()
		{
			char username[256];
			unsigned long usernameLen = 256;
			GetUserNameA(username, &usernameLen);
			std::filesystem::path userSettingsPath = std::string("C:\\users\\") + username + "\\AppData\\Local";
			return userSettingsPath;
		}
	}
}
