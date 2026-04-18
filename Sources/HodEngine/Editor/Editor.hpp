#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp>
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

	/// @brief
	class HOD_EDITOR_API Editor
	{
		_Singleton(Editor)

	public:
		~Editor();

	public:
		bool Init(const ArgumentParser& argumentParser);

		bool CreateProject(const Path& path);
		bool OpenProject(const Path& path);

		bool LoadEditor();

		bool Save();
		bool SaveSceneAs();

		void Build();
		void BuildAndRun();

		template<typename _EditorTabType_>
		_EditorTabType_* OpenAsset(std::shared_ptr<Asset> asset);
		EditorTab*       OpenAsset(std::shared_ptr<Asset> asset);

		template<typename _EditorTabType_>
		_EditorTabType_* OpenEditorTab();

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

		Texture* GetSerializedDataTexture() const
		{
			return _serializedDataTexture;
		}

		Texture* GetCheckerTexture() const
		{
			return _checkerTexture;
		}

	private:
		bool AddProjectInRecentProject(const Path& path) const;

		bool LoadEditorModules();
		bool UnloadEditorModules();

	private:
		Vector<DynamicLibrary*> _editorModules;

		Vector<EditorTab*>                                                            _editorTabs;
		std::unordered_map<String, std::function<EditorTab*(std::shared_ptr<Asset>)>> _editorTabFactory;

		Texture* _hodTexture = nullptr;
		Texture* _folderTexture = nullptr;
		Texture* _folderOpenTexture = nullptr;
		Texture* _sceneTexture = nullptr;
		Texture* _prefabTexture = nullptr;
		Texture* _serializedDataTexture = nullptr;
		Texture* _checkerTexture = nullptr;

		FpsCounter _fpsCounter;

		bool                _showFloatingAssetBrowser = false;
		bool                _focusFloatingAssetBrowserWindow = false;
		float               _floatingAssetBrowserWindowPos = 0.0f;
		AssetBrowserWindow* _floatingAssetBrowserWindow = nullptr;
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
