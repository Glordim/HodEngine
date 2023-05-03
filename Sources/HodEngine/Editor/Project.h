#pragma once

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/Singleton.h>
#include <HodEngine/Core/Reflection/ReflectionMacros.h>

namespace hod::editor
{
	/// @brief 
	class Project
	{
		REFLECTED_CLASS(Project)

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
