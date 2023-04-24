#pragma once

#include <stdint.h>

#include <HodEngine/Core/Src/Singleton.h>
#include <HodEngine/Core/Src/Event.h>

#include <filesystem>

#include "HodEngine/Editor/src/AssetDatabase.h"

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

	private:

		bool			AddProjectInRecentProject(const std::filesystem::path& path) const;

	private:

		MainBar*		_mainBar = nullptr;
	};
}
