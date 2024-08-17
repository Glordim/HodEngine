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

		void									SetEntitySelection(std::weak_ptr<game::Entity> selection);
		void									SetAssetSelection(const AssetDatabase::FileSystemMapping* selection);

		const std::weak_ptr<game::Entity>&		GetEntitySelection() const;
		const AssetDatabase::FileSystemMapping* GetAssetSelection() const;

		void									OpenAsset(std::shared_ptr<Asset> asset);

		void									Play();
		void									Stop();
		void									Pause();
		void									Resume();
		void									PlayNextFrame();

		bool									IsPlaying() const;
		bool									IsPaused() const;

		Asset*									GetCurrentScene() const;
		void									MarkCurrentSceneAsDirty();

		renderer::Texture*						GetFolderTexture() const { return _folderTexture; }
		renderer::Texture*						GetFolderOpenTexture() const  { return _folderOpenTexture; }
		renderer::Texture*						GetSceneTexture() const  { return _sceneTexture; }
		renderer::Texture*						GetPrefabTexture() const  { return _prefabTexture; }
		renderer::Texture*						GetCheckerTexture() const  { return _checkerTexture; }

	private:

		bool									AddProjectInRecentProject(const std::filesystem::path& path) const;

	private:

		MainBar*								_mainBar = nullptr;

		std::weak_ptr<game::Entity> 			_entitySelection;
		const AssetDatabase::FileSystemMapping* _assetSelection = nullptr;

		Asset*									_currentScene = nullptr;

		bool									_playing = false;
		bool									_paused = false;

		renderer::Texture*						_folderTexture = nullptr;
		renderer::Texture*						_folderOpenTexture = nullptr;
		renderer::Texture*						_sceneTexture = nullptr;
		renderer::Texture*						_prefabTexture = nullptr;
		renderer::Texture*						_checkerTexture = nullptr;

		Document								_playedWorldDocument;
	};
}
