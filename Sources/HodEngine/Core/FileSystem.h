#pragma once

#include <filesystem>

namespace hod
{
	/// @brief 
	class FileSystem
	{
	public:

		static std::filesystem::path GetUserSettingsPath();

	private:

		static std::filesystem::path	_userSettingsPath;
	};
}
