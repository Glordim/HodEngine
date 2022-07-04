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

		bool							Load();
		bool							Save();

		const std::filesystem::path&	GetAssetDirPath() const;

	private:

		std::filesystem::path			_projectPath;
		std::filesystem::path			_assetDirPath;

		std::string						_name;
	};
}
