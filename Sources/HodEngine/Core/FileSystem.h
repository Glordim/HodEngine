#pragma once

#include <filesystem>

namespace hod
{
	/// @brief 
	class FileSystem
	{
	public:

		static std::filesystem::path	GetUserSettingsPath();
		static std::filesystem::path	GetExecutablePath();
		static bool						SetWorkingDirectory(const std::filesystem::path& path);

	private:

		static std::filesystem::path	_userSettingsPath;
		static std::filesystem::path	_executablePath;
	};
}
