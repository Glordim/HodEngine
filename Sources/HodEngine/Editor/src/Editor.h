#pragma once

#include <stdint.h>

#include <HodEngine/Core/Src/Singleton.h>

#include <filesystem>

namespace hod::editor
{
	class MainBar;
	class Project;

	/// @brief 
	class Editor : public Singleton<Editor>
	{
		friend class Singleton<Editor>;

	public:

		bool		Init();

		bool		CreateProject(const std::filesystem::path& path);
		bool		OpenProject(const std::filesystem::path& path);
		bool		CloseProject();

		Project*	GetProject();

	protected:

					Editor();
					~Editor();

	private:

		MainBar*	_mainBar = nullptr;
		Project*	_project = nullptr;
	};
}
