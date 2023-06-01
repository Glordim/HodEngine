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

namespace hod::editor
{
	class MainBar;
	class Project;

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

		void									SetSceneSelection(void* selection);
		void									SetAssetSelection(const AssetDatabase::FileSystemMapping* selection);

		void*									GetSceneSelection() const;
		const AssetDatabase::FileSystemMapping* GetAssetSelection() const;

	private:

		bool			AddProjectInRecentProject(const std::filesystem::path& path) const;

	private:

		MainBar*		_mainBar = nullptr;

		void* 									_sceneSelection = nullptr;
		const AssetDatabase::FileSystemMapping* _assetSelection = nullptr;
	};
}
