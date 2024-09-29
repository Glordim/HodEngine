#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Module/Module.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::editor
{
	class Asset;

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

		void							SetStartupScene(std::shared_ptr<Asset> asset);
		const UID&						GetStartupScene() const;

		const std::filesystem::path&	GetProjectPath() const;
		const std::filesystem::path&	GetAssetDirPath() const;
		const std::filesystem::path&	GetResourceDirPath() const;
		const std::filesystem::path&	GetThumbnailDirPath() const;
		const std::filesystem::path&	GetBuildsDirPath() const;

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
		std::filesystem::path			_buildsDirPath;

		std::string						_name;
		UID								_startupScene;

		Module							_gameModule;
		FileSystemWatcher				_gameModuleFileSystemWatcher;
	};
}
