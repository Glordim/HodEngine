#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include <filesystem>

#include "HodEngine/Editor/AssetDatabase.hpp"

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

		ViewportWindow*							GetCurrentViewport() const;
		void									SetCurrentViewport(ViewportWindow* viewportWindow);

		
		template<typename _EditorTabType_>
		_EditorTabType_*						OpenAsset(std::shared_ptr<Asset> asset);
		EditorTab*								OpenAsset(std::shared_ptr<Asset> asset);

		template<typename _EditorTabType_>
		_EditorTabType_*						OpenEditorTab();

		/*
		void									Play();
		void									Stop();
		void									Pause();
		void									Resume();
		void									PlayNextFrame();

		bool									IsPlaying() const;
		bool									IsPaused() const;

		Asset*									GetCurrentScene() const;
		void									MarkCurrentSceneAsDirty();
		*/

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

		ViewportWindow*							_currentViewport = nullptr;

		Asset*									_currentScene = nullptr;
		/*
		bool									_playing = false;
		bool									_paused = false;
		*/
		renderer::Texture*						_folderTexture = nullptr;
		renderer::Texture*						_folderOpenTexture = nullptr;
		renderer::Texture*						_sceneTexture = nullptr;
		renderer::Texture*						_prefabTexture = nullptr;
		renderer::Texture*						_serializedDataTexture = nullptr;
		renderer::Texture*						_shaderTexture = nullptr;
		renderer::Texture*						_checkerTexture = nullptr;

		Document								_playedWorldDocument;
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
