#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>
#include <stdint.h>
#include <string_view>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Singleton.hpp>

#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/FpsCounter.hpp"
#include "HodEngine/Editor/Project.hpp"

namespace hod::inline core
{
	class ArgumentParser;
}

namespace hod::inline game
{
	class Actor;
	class Entity;
}

namespace hod::inline rhi
{
	class Texture;
}

namespace hod::inline editor
{
	class MainBar;
	class Project;
	class Asset;
	class EditorTab;
	class AssetBrowserWindow;
	class TaskTrackerWindow;
	class TaskTracker;

	/// @brief
	class HOD_EDITOR_API Editor
	{
		_Singleton(Editor)

	public:
		enum class BuildPlatform
		{
			Windows,
			Macos,
			Linux,
			Android,
		};

	public:
		~Editor();

	public:
		bool Init(const ArgumentParser& argumentParser);

		/// @brief Creates the project, then opens it
		/// @param versionControl A provider's name: its workspace is created on the first connection (see
		/// VersionControl::RequestWorkspaceCreation). Empty: none chosen (the automatic choice applies)
		bool CreateProject(const Project::CreateInfo& createInfo, std::string_view versionControl);
		bool OpenProject(const Path& path);

		bool LoadEditor();

		bool Save();

		void OpenImportDialog(const Path& path);
		void Import(const Path& sourceFilePath, const Path& destinationDirPath);
		void Import(const Vector<Path>& sourceFilePaths, const Path& destinationDirPath);

		void Cook(const Path& asset);

		void Build(BuildPlatform buildPlatform);
		void BuildAndRun();

		template<typename _EditorTabType_>
		_EditorTabType_* OpenAsset(std::shared_ptr<Asset> asset);
		EditorTab*       OpenAsset(std::shared_ptr<Asset> asset);

		template<typename _EditorTabType_>
		_EditorTabType_* OpenEditorTab();

		/// @brief By the editor modules when they unload: closes the tabs of the assets of this type (AssetType::CreateEditorTab), their
		/// code goes with the module (unsaved changes are lost)
		void CloseEditorTabsOf(uint64_t assetType);

		void PingAsset(std::shared_ptr<Asset> asset);

		const Vector<EditorTab*>& GetEditorTabs() const
		{
			return _editorTabs;
		}
		// The tab shown in the tab bar, nullptr if none
		EditorTab* GetSelectedEditorTab() const;

		Texture* GetHodTexture() const
		{
			return _hodTexture;
		}

		Texture* GetFolderTexture() const
		{
			return _folderTexture;
		}

		Texture* GetSceneTexture() const
		{
			return _sceneTexture;
		}

		Texture* GetPrefabTexture() const
		{
			return _prefabTexture;
		}

		Texture* GetMaterialTexture() const
		{
			return _materialTexture;
		}

		Texture* GetFontTexture() const
		{
			return _fontTexture;
		}

		Texture* GetAudioTexture() const
		{
			return _audioTexture;
		}

		Texture* GetDataItemTexture() const
		{
			return _dataItemTexture;
		}

		Texture* GetDataTableTexture() const
		{
			return _dataTableTexture;
		}

		/// @brief 2x2 transparency checker (Nearest + Repeat), CheckerPrimaryGrey in its top-left texel
		Texture* GetCheckerTexture() const
		{
			return _checkerTexture;
		}

		/// @brief Grey levels of the transparency checker, shared by GetCheckerTexture() and the baked thumbnail backgrounds
		static constexpr uint8_t CheckerPrimaryGrey = 71;
		static constexpr uint8_t CheckerSecondaryGrey = 102;

		TaskTracker& GetTaskTracker()
		{
			return *_taskTracker;
		}

		/// @brief Closes the editor, then starts it again on the project once this one is gone (see PluginsWindow)
		void Restart();

		/// @brief A tab has changes not saved
		bool HasDirtyTabs() const;

		/// @brief Its runtime library couldn't be loaded or initialized (see LoadRuntimeModules): its editor library isn't loaded
		bool HasModuleFailed(const String& name) const;

	private:
		bool AddProjectInRecentProject(const Path& path) const;

		/// @brief The runtime libraries of the project's enabled modules (see Project::GetModuleGraph): loaded, registered, initialized
		bool LoadRuntimeModules();
		void UnloadRuntimeModules();

		/// @brief The editor libraries of the project's enabled modules, in the modules' order
		bool LoadEditorModules();
		bool UnloadEditorModules();

	private:
		Event<>::Slot _onProjectModulesReloadedSlot;

		// What an editor module registered goes with it (see HOD_STARTUP_EDITOR_MODULE)
		struct EditorModule
		{
			DynamicLibrary*           _library = nullptr;
			ModuleRegistration*       _registration = nullptr;
		};
		Vector<EditorModule> _editorModules;

		// The runtime libraries of the project's optional modules (see RuntimeModuleHost), for the editor's life
		Vector<DynamicLibrary*>  _runtimeModuleLibraries;
		RuntimeModuleHost*       _runtimeModuleHost = nullptr;
		Vector<String>           _failedModules; // Whose runtime library couldn't be loaded or initialized: their editor library isn't loaded
		bool                     _modulesLoaded = false;

		bool _restartRequested = false;
		Path _restartProjectPath;

		Vector<EditorTab*>                                                            _editorTabs;

		Texture* _hodTexture = nullptr;
		Texture* _folderTexture = nullptr;
		Texture* _sceneTexture = nullptr;
		Texture* _prefabTexture = nullptr;
		Texture* _materialTexture = nullptr;
		Texture* _fontTexture = nullptr;
		Texture* _audioTexture = nullptr;
		Texture* _dataItemTexture = nullptr;
		Texture* _dataTableTexture = nullptr;
		Texture* _checkerTexture = nullptr;

		FpsCounter _fpsCounter;

		bool                _showFloatingAssetBrowser = false;
		bool                _focusFloatingAssetBrowserWindow = false;
		float               _floatingAssetBrowserWindowPos = 0.0f;
		AssetBrowserWindow* _floatingAssetBrowserWindow = nullptr;

		bool               _showFloatingTaskTracker = false;
		bool               _focusFloatingTaskTrackerWindow = false;
		float              _floatingTaskTrackerWindowPos = 0.0f;
		TaskTrackerWindow* _floatingTaskTrackerWindow = nullptr;

		TaskTracker* _taskTracker = nullptr;
	};

	template<typename _EditorTabType_>
	_EditorTabType_* Editor::OpenAsset(std::shared_ptr<Asset> asset)
	{
		return static_cast<_EditorTabType_*>(OpenAsset(asset)); // todo check cast compat ?
	}

	template<typename _EditorTabType_>
	_EditorTabType_* Editor::OpenEditorTab()
	{
		_EditorTabType_* editorTab = DefaultAllocator::GetInstance().New<_EditorTabType_>(nullptr);
		_editorTabs.push_back(editorTab);
		return editorTab;
	}
}
