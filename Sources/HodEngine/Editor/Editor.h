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

		void									SetEntitySelection(std::weak_ptr<game::Entity> selection);
		void									SetAssetSelection(const AssetDatabase::FileSystemMapping* selection);

		const std::weak_ptr<game::Entity>&		GetEntitySelection() const;
		const AssetDatabase::FileSystemMapping* GetAssetSelection() const;

		void									OpenAsset(Asset& asset);

	private:

		bool			AddProjectInRecentProject(const std::filesystem::path& path) const;

	private:

		MainBar*		_mainBar = nullptr;

		std::weak_ptr<game::Entity> 			_entitySelection;
		const AssetDatabase::FileSystemMapping* _assetSelection = nullptr;
	};
}
