#pragma once

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/Src/Singleton.h>

namespace hod::editor
{
	/// @brief 
	class Project
	{
		_Singleton(Project)

	public:

		bool							Create(const std::filesystem::path& directory);
		bool							Open(const std::filesystem::path& projectPath);

		bool							Load();
		bool							Save();

		const std::filesystem::path&	GetProjectPath();
		const std::filesystem::path&	GetAssetDirPath();

	private:

		std::filesystem::path			_projectPath;
		std::filesystem::path			_assetDirPath;

		std::string						_name;
	};
}
