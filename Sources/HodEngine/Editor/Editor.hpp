#pragma once

#include <stdint.h>

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Event.hpp>

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

namespace hod::editor
{
	class MainBar;
	class Project;
	class Asset;

	/// @brief 
	class Editor
	{
		_Singleton(Editor)

	public:

						~Editor();

	public:

		bool			Init(const ArgumentParser& argumentParser);

		bool			CreateProject(const std::filesystem::path& path);
		bool			OpenProject(const std::filesystem::path& path);

		bool			Save();
		bool			SaveSceneAs();

		void									SetEntitySelection(std::weak_ptr<game::Entity> selection);
		void									SetAssetSelection(const AssetDatabase::FileSystemMapping* selection);

		const std::weak_ptr<game::Entity>&		GetEntitySelection() const;
		const AssetDatabase::FileSystemMapping* GetAssetSelection() const;

		void									OpenAsset(Asset& asset);

		void									Play();
		void									Stop();
		void									Pause();
		void									Resume();
		void									PlayNextFrame();

		bool									IsPlaying() const;
		bool									IsPaused() const;

	private:

		bool			AddProjectInRecentProject(const std::filesystem::path& path) const;

	private:

		MainBar*		_mainBar = nullptr;

		std::weak_ptr<game::Entity> 			_entitySelection;
		const AssetDatabase::FileSystemMapping* _assetSelection = nullptr;

		Asset*									_currentScene = nullptr;

		bool									_playing = false;
		bool									_paused = false;
	};
}
