#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Module/Module.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API Project
	{
		REFLECTED_CLASS_NO_VIRTUAL(Project, HOD_EDITOR_API)

		_Singleton(Project)

	public:

		bool							Create(const std::filesystem::path& directory);
		bool							Open(const std::filesystem::path& projectPath);

		bool							Load();
		bool							Save();

		const std::filesystem::path&	GetProjectPath() const;
		const std::filesystem::path&	GetAssetDirPath() const;
		const std::filesystem::path&	GetResourceDirPath() const;
		const std::filesystem::path&	GetThumbnailDirPath() const;

		bool							HasGameModule() const;

		bool							GenerateGameModuleCMakeList() const;
		bool							ConfigureGameModule() const;
		bool							BuildGameModule() const;

		bool							ReloadGameModule();

	private:

		std::filesystem::path			_projectPath;
		std::filesystem::path			_assetDirPath;
		std::filesystem::path			_resourceDirPath;
		std::filesystem::path			_thumbnailDirPath;

		std::string						_name;

		Module							_gameModule;
		FileSystemWatcher				_gameModuleFileSystemWatcher;
	};
}
