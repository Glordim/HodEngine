#pragma once

#include <stdint.h>

#include <HodEngine/Core/Src/Singleton.h>
#include <HodEngine/Core/Src/Event.h>

#include <filesystem>

#include "HodEngine/Editor/src/AssetDatabase.h"

namespace hod::editor
{
	class MainBar;
	class Project;

	/// @brief 
	class Editor : public Singleton<Editor>
	{
		friend class Singleton<Editor>;

	public:

		Event<Project*>	OnProjectLoaded;
		Event<Project*>	OnProjectClosed;

	public:

		bool			Init();

		bool			CreateProject(const std::filesystem::path& path);
		bool			OpenProject(const std::filesystem::path& path);
		bool			CloseProject();

		Project*		GetProject();
		AssetDatabase&	GetAssetDatabase();

	protected:

						Editor();
						~Editor();

	private:

		MainBar*		_mainBar = nullptr;
		Project*		_project = nullptr;
		AssetDatabase	_assetDatabase;
	};
}
