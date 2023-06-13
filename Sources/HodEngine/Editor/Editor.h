#pragma once

#include <stdint.h>

#include <HodEngine/Core/Singleton.h>
#include <HodEngine/Core/Event.h>

#include <filesystem>

#include "HodEngine/Editor/AssetDatabase.h"

namespace hod::core
{
	class ArgumentParser;
}

namespace hod::game
{
	class Actor;
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

		bool			Init(const core::ArgumentParser& argumentParser);

		bool			CreateProject(const std::filesystem::path& path);
		bool			OpenProject(const std::filesystem::path& path);

		bool			Save();

		void									SetActorSelection(game::Actor* selection);
		void									SetAssetSelection(const AssetDatabase::FileSystemMapping* selection);

		game::Actor*							GetActorSelection() const;
		const AssetDatabase::FileSystemMapping* GetAssetSelection() const;

		void									OpenAsset(Asset& asset);

	private:

		bool			AddProjectInRecentProject(const std::filesystem::path& path) const;

	private:

		MainBar*		_mainBar = nullptr;

		game::Actor* 							_actorSelection = nullptr;
		const AssetDatabase::FileSystemMapping* _assetSelection = nullptr;
	};
}
