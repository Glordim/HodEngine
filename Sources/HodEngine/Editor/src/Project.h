#pragma once

#include <stdint.h>

#include <filesystem>

namespace hod::editor
{
	class MainBar;

	/// @brief 
	class Project
	{
	public:
								Project(const std::filesystem::path& projectPath);
								~Project();

		bool					Load();
		bool					Save();

	private:

		std::filesystem::path	_projectPath;
		std::filesystem::path	_assetPath;

		std::string				_name;
	};
}
