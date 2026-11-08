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

#include "HodEngine/Editor/Modules/ModuleGraph.hpp"

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

		static bool IsValidName(const String& name);
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
		const Path& GetCacheBuildDirPath() const;
		const Path& GetBuildDirPath() const;
		const Path& GetTmpDirPath() const;
		const Path& GetProjectSettingsDirPath() const;
		/// @brief The C++ sources of the game and editor modules (Code/<Name>Game, Code/<Name>Editor)
		const Path& GetCodeDirPath() const;

		bool HasGameModule() const;
		bool CanLoadProjectModules();
		Path GetGameModulePath() const;

		bool GenerateGameModuleCMakeList() const;
		bool ConfigureGameModule() const;
		bool BuildGameModule() const;

		bool ReloadProjectModules();

		/// @brief The engine modules (see ModuleGraph), resolved against the project's disabled ones when it is opened
		const ModuleGraph&    GetModuleGraph() const;
		const Vector<String>& GetDisabledModules() const;

		/// @brief Saved in the project file (checked out first if the version control needs it). Taken into account when the project
		/// is opened again (see PluginsWindow): the module graph stays the one the project was opened with
		/// @return false (with an error) if the project file can't be written
		bool SetDisabledModules(const Vector<String>& disabledModules);

		bool CreateMinimalSourceForModule(const Path& directory);

		Event<>& GetOnModulesReloadedEvent() { return _onModulesReloadedEvent; }

	private:
		static bool WriteModuleSourceFile(const Path& path, std::string_view templateContent, const String& moduleName, const String& macroPrefix);

		void ResolveModules();

		/// @brief What the game module built for the editor links (see BuildGameModule): its runtime libraries, one per line
		Path   GetGameModuleRuntimeLibrariesStampPath() const;
		String GetRuntimeLibrariesStamp() const;

	private:
		Path _projectPath;
		Path _sourceDirPath;
		Path _assetDirPath;
		Path _resourceDirPath;
		Path _cacheBuildDirPath;
		Path _buildDirPath;
		Path _tmpDirPath;
		Path _projectSettingsDirPath; // Of the ProjectSettingsManager
		Path _codeDirPath;

		String _name;
		UID    _startupScene;

		Vector<String> _disabledModules; // Names of the engine modules the project doesn't use (see ModuleGraph::Resolve)
		ModuleGraph    _moduleGraph;

		DynamicLibrary    _gameModule;
		FileSystemWatcher _gameModuleFileSystemWatcher;

		DynamicLibrary    _editorModule;
		FileSystemWatcher _editorModuleFileSystemWatcher;

		Event<> _onModulesReloadedEvent;
	};
}
