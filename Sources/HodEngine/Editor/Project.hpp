#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

#include <HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/UID.hpp>

#include "HodEngine/Editor/Plugins/PluginGraph.hpp"

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
		/// @brief What a new project is made of (see NewProjectForm)
		struct CreateInfo
		{
			Path           _directory;       // The project folder: <destination>/<name>, its name is the project's
			Vector<String> _disabledPlugins; // As the project's _disabledPlugins (see PluginGraph::Resolve)
		};

	public:
		~Project();

		/// @brief The folder, the minimal sources of the game and editor modules, the project file. Not opened
		/// @return false (with an error) if the folder isn't a valid new project folder (see CheckNewProjectDirectory) or can't be written
		bool Create(const CreateInfo& createInfo);

		static bool IsValidName(const String& name);
		/// @brief Why directory can't be a new project folder (its name isn't valid, it's a file, a folder with something in it), empty if it can
		static String CheckNewProjectDirectory(const Path& directory);
		/// @brief The plugins of this editor (the manifests next to it, see hod_plugin_manifest), built, everything enabled
		/// @return false if some were left out (see PluginGraph::GetErrors)
		static bool BuildEngineModuleGraph(PluginGraph& moduleGraph);
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

		/// @brief The plugins (see PluginGraph), resolved against the project's disabled ones when it is opened, the project's own last
		const PluginGraph&    GetPluginGraph() const;

		/// @brief The project's own plugin, last of the graph (see PluginGraph::AppendLast): its game module (a runtime module) and its
		/// editor module (see HOD_STARTUP_MODULE), built from its sources in its build directory, loaded by the editor (see
		/// Editor::LoadProjectPlugin). Null if it couldn't be added (a plugin has the project's name)
		const PluginManifest* GetProjectPlugin() const;
		const Vector<String>& GetDisabledPlugins() const;

		/// @brief Saved in the project file (checked out first if the version control needs it). Taken into account when the project
		/// is opened again (see PluginsWindow): the plugin graph stays the one the project was opened with
		/// @return false (with an error) if the project file can't be written
		bool SetDisabledPlugins(const Vector<String>& disabledPlugins);

		bool CreateMinimalSourceForModule(const Path& directory);

		/// @brief Its game module or its editor module was rebuilt (their libraries changed): the editor reloads them
		Event<>& GetOnModulesRebuiltEvent() { return _onModulesRebuiltEvent; }

		/// @brief Its plugin's modules were (re)loaded by the editor (see Editor::ReloadProjectPlugin), what they register is there
		Event<>& GetOnModulesReloadedEvent() { return _onModulesReloadedEvent; }

	private:
		static bool WriteModuleSourceFile(const Path& path, std::string_view templateContent, const String& moduleName, const String& macroPrefix);

		void ResolvePlugins();

		/// @brief What the game module built for the editor links (see BuildGameModule): its runtime modules, one per line
		Path   GetGameModuleRuntimeModulesStampPath() const;
		String GetRuntimeModulesStamp() const;

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

		Vector<String> _disabledPlugins; // Names of the plugins the project doesn't use (see PluginGraph::Resolve)
		PluginGraph    _pluginGraph;

		Path _modulesDirectoryPath; // Where its game module and its editor module are built

		// Their library, as DynamicLibrary::Init takes it (no prefix, no extension, see DynamicLibrary::GetFilePath)
		Path              _gameModuleLibraryPath;
		FileSystemWatcher _gameModuleFileSystemWatcher;
		Path              _editorModuleLibraryPath;
		FileSystemWatcher _editorModuleFileSystemWatcher;

		Event<> _onModulesRebuiltEvent;
		Event<> _onModulesReloadedEvent;
	};
}
