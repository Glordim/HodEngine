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

namespace hod::inline core
{
	class ModuleRegistration;
	class RuntimeModuleHost;
}

namespace hod::inline editor
{
	class Asset;

	/// @brief
	class HOD_EDITOR_API Project
	{
		REFLECTED_CLASS_NO_VIRTUAL(Project)

		_Singleton(Project)

	public:
		/// @brief What a new project is made of (see NewProjectForm)
		struct CreateInfo
		{
			Path           _directory;       // The project folder: <destination>/<name>, its name is the project's
			Vector<String> _disabledModules; // As the project's _disabledModules (see ModuleGraph::Resolve)
		};

	public:
		~Project();

		/// @brief The folder, the minimal sources of the game and editor modules, the project file. Not opened
		/// @return false (with an error) if the folder isn't a valid new project folder (see CheckNewProjectDirectory) or can't be written
		bool Create(const CreateInfo& createInfo);

		static bool IsValidName(const String& name);
		/// @brief Why directory can't be a new project folder (its name isn't valid, it's a file, a folder with something in it), empty if it can
		static String CheckNewProjectDirectory(const Path& directory);
		/// @brief The engine modules of this editor (the manifests next to it, see hod_module_manifest), built, everything enabled
		/// @return false if some were left out (see ModuleGraph::GetErrors)
		static bool BuildEngineModuleGraph(ModuleGraph& moduleGraph);
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

		/// @brief The game module (a runtime library, see RuntimeModuleHost) and its editor library (an editor module, see HOD_STARTUP_EDITOR_MODULE),
		/// loaded after the engine's
		bool LoadProjectModules();
		void UnloadProjectModules();
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

		DynamicLibrary          _gameModule;
		FileSystemWatcher       _gameModuleFileSystemWatcher;
		RuntimeModuleHost*       _gameModuleHost = nullptr; // While the game module is loaded

		DynamicLibrary            _editorModule;
		FileSystemWatcher         _editorModuleFileSystemWatcher;
		ModuleRegistration*       _editorModuleRegistration = nullptr; // While the editor module is loaded

		Event<> _onModulesReloadedEvent;
	};
}
