#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

#include <HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp>
#include <HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/UID.hpp>

#include <memory>

namespace hod::inline editor
{
	class Asset;

	/// @brief
	class HOD_EDITOR_API Project
	{
		REFLECTED_CLASS_NO_VIRTUAL(Project)

		_Singleton(Project)

	public:
		~Project();

		bool Create(const Path& directory);
		bool Open(const Path& projectPath);

		bool Load();
		bool Save();

		void       SetStartupScene(std::shared_ptr<Asset> asset);
		const UID& GetStartupScene() const;

		const String GetName() const;

		const Path& GetProjectPath() const;
		const Path& GetSourceDirPath() const;
		const Path& GetAssetDirPath() const;
		const Path& GetResourceDirPath() const;
		const Path& GetThumbnailDirPath() const;
		const Path& GetCacheBuildDirPath() const;
		const Path& GetBuildDirPath() const;

		bool HasGameModule() const;
		Path GetGameModulePath() const;

		bool GenerateGameModuleCMakeList() const;
		bool ConfigureGameModule() const;
		bool BuildGameModule() const;

		bool ReloadProjectModules();

		bool CreateMinimalSourceForModule(const Path& directory);

		Event<>& GetOnModulesReloadedEvent() { return _onModulesReloadedEvent; }

	private:
		Path _projectPath;
		Path _sourceDirPath;
		Path _assetDirPath;
		Path _resourceDirPath;
		Path _thumbnailDirPath;
		Path _cacheBuildDirPath;
		Path _buildDirPath;

		String _name;
		UID    _startupScene;

		DynamicLibrary    _gameModule;
		FileSystemWatcher _gameModuleFileSystemWatcher;

		DynamicLibrary    _editorModule;
		FileSystemWatcher _editorModuleFileSystemWatcher;

		Event<> _onModulesReloadedEvent;
	};
}
