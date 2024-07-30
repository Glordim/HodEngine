#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <Windows.h>

namespace hod
{
	std::filesystem::path FileSystem::_userSettingsPath;
	std::filesystem::path FileSystem::_executablePath;

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
}
