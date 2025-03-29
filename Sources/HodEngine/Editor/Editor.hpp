#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include <filesystem>

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/FpsCounter.hpp"

namespace hod
{
	class ArgumentParser;
}

namespace hod::game
{
	class Actor;
	class Entity;
}

namespace hod::renderer
{
	class Texture;
}

namespace hod::editor
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

		bool									Init(const ArgumentParser& argumentParser);

		bool									CreateProject(const std::filesystem::path& path);
		bool									OpenProject(const std::filesystem::path& path);

		bool									LoadEditor();

		bool									Save();
		bool									SaveSceneAs();

		void									Build();
		void									BuildAndRun();

		void									SetAssetSelection(const AssetDatabase::FileSystemMapping* selection);

		const AssetDatabase::FileSystemMapping* GetAssetSelection() const;

		
		template<typename _EditorTabType_>
		_EditorTabType_*						OpenAsset(std::shared_ptr<Asset> asset);
		EditorTab*								OpenAsset(std::shared_ptr<Asset> asset);

		template<typename _EditorTabType_>
		_EditorTabType_*						OpenEditorTab();

		renderer::Texture*						GetHodTexture() const  { return _hodTexture; }
		renderer::Texture*						GetFolderTexture() const { return _folderTexture; }
		renderer::Texture*						GetFolderOpenTexture() const  { return _folderOpenTexture; }
		renderer::Texture*						GetSceneTexture() const  { return _sceneTexture; }
		renderer::Texture*						GetPrefabTexture() const  { return _prefabTexture; }
		renderer::Texture*						GetSerializedDataTexture() const  { return _serializedDataTexture; }
		renderer::Texture*						GetShaderTexture() const  { return _shaderTexture; }
		renderer::Texture*						GetCheckerTexture() const  { return _checkerTexture; }

	private:

		bool									AddProjectInRecentProject(const std::filesystem::path& path) const;

	private:

		MainBar*								_mainBar = nullptr;
		std::vector<EditorTab*>					_editorTabs;
		std::unordered_map<std::string, std::function<EditorTab*(std::shared_ptr<Asset>)>> _editorTabFactory;

		const AssetDatabase::FileSystemMapping* _assetSelection = nullptr;

		renderer::Texture*						_hodTexture = nullptr;
		renderer::Texture*						_folderTexture = nullptr;
		renderer::Texture*						_folderOpenTexture = nullptr;
		renderer::Texture*						_sceneTexture = nullptr;
		renderer::Texture*						_prefabTexture = nullptr;
		renderer::Texture*						_serializedDataTexture = nullptr;
		renderer::Texture*						_shaderTexture = nullptr;
		renderer::Texture*						_checkerTexture = nullptr;

		FpsCounter								_fpsCounter;

		bool									_showFloatingAssetBrowser = false;
		float									_floatingAssetBrowserWindowPos = 0.0f;
		AssetBrowserWindow*						_floatingAssetBrowserWindow = nullptr;
	};

	template<typename _EditorTabType_>
	_EditorTabType_* Editor::OpenAsset(std::shared_ptr<Asset> asset)
	{
		return static_cast<_EditorTabType_*>(OpenAsset(asset)); // todo check cast compat ?
	}

	template<typename _EditorTabType_>
	_EditorTabType_* Editor::OpenEditorTab()
	{
		_EditorTabType_* editorTab = new _EditorTabType_(nullptr);
		_editorTabs.push_back(editorTab);
		return editorTab;
	}
}
