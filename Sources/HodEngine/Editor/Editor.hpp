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

namespace hod::inline core
{
	class ArgumentParser;
}

namespace hod::inline game
{
	class Actor;
	class Entity;
}

namespace hod::inline renderer
{
	class Texture;
}

namespace hod::inline editor
{
	class MainBar;
	class Project;
	class Asset;
	class ViewportWindow;
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

		bool CreateProject(const Path& path);
		bool OpenProject(const Path& path);

		bool LoadEditor();

		bool Save();
		bool SaveSceneAs();

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

		template<typename _EditorTabType_>
		void RegisterEditorTab(std::string_view name);

		void PingAsset(std::shared_ptr<Asset> asset);

		Texture* GetHodTexture() const
		{
			return _hodTexture;
		}

		Texture* GetFolderTexture() const
		{
			return _folderTexture;
		}

		Texture* GetFolderOpenTexture() const
		{
			return _folderOpenTexture;
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

		Texture* GetCheckerTexture() const
		{
			return _checkerTexture;
		}

		TaskTracker& GetTaskTracker()
		{
			return *_taskTracker;
		}

	private:
		bool AddProjectInRecentProject(const Path& path) const;

		bool LoadEditorModules();
		bool UnloadEditorModules();

	private:
		Event<>::Slot _onProjectModulesReloadedSlot;

		Vector<DynamicLibrary*> _editorModules;

		Vector<EditorTab*>                                                            _editorTabs;
		std::unordered_map<uint64_t, std::function<EditorTab*(std::shared_ptr<Asset>)>> _editorTabFactory;

		Texture* _hodTexture = nullptr;
		Texture* _folderTexture = nullptr;
		Texture* _folderOpenTexture = nullptr;
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

	template<typename _EditorTabType_>
	void Editor::RegisterEditorTab(std::string_view name)
	{
		_editorTabFactory.emplace(Hash::ComputeXxh3_64(name), [](std::shared_ptr<Asset> asset) { return DefaultAllocator::GetInstance().New<_EditorTabType_>(asset); });
	}
}
