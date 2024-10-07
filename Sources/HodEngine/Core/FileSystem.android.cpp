#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod
{
	std::filesystem::path FileSystem::_userSettingsPath;
	std::filesystem::path FileSystem::_executablePath;

	std::filesystem::path FileSystem::GetUserSettingsPath()
	{
		return FileSystem::_userSettingsPath;
	}

	std::filesystem::path FileSystem::GetExecutablePath()
	{
		return FileSystem::_executablePath;
	}

	bool FileSystem::SetWorkingDirectory(const std::filesystem::path& path)
	{
		return false;
	}
}
