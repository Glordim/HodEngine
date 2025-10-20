#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

#include <HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp>
#include <HodEngine/Core/Module/Module.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::editor
{
	class Asset;

	/// @brief
	class HOD_EDITOR_API Project
	{
		REFLECTED_CLASS_NO_VIRTUAL(Project)

		_Singleton(Project)

	public:
		bool Create(const Path& directory);
		bool Open(const Path& projectPath);

		bool Load();
		bool Save();

		void       SetStartupScene(std::shared_ptr<Asset> asset);
		const UID& GetStartupScene() const;

		const String GetName() const;

		const Path& GetProjectPath() const;
		const Path& GetAssetDirPath() const;
		const Path& GetResourceDirPath() const;
		const Path& GetThumbnailDirPath() const;
		const Path& GetBuildsDirPath() const;

		bool HasGameModule() const;
		Path GetGameModulePath() const;

		bool GenerateGameModuleCMakeList() const;
		bool ConfigureGameModule() const;
		bool BuildGameModule() const;

		bool ReloadGameModule();

		bool CreateMinimalSourceForModule(const Path& directory);

	private:
		Path _projectPath;
		Path _assetDirPath;
		Path _resourceDirPath;
		Path _thumbnailDirPath;
		Path _buildsDirPath;

		String _name;
		UID    _startupScene;

		Module            _gameModule;
		FileSystemWatcher _gameModuleFileSystemWatcher;
	};
}
