#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Editor/NewProjectForm.hpp"
#include "HodEngine/Editor/RecentProjects.hpp"

namespace hod::inline editor
{
	/// @brief
	class HOD_EDITOR_API ProjectBrowser : public imgui::Window
	{
		REFLECTED_CLASS(ProjectBrowser, imgui::Window)

	public:
		ProjectBrowser();
		~ProjectBrowser() override = default;

	public:
		bool Draw() override;
		void DrawContent() override {};

	private:
		void OpenProject(const Path& path);
		void ShowNewProjectForm(bool show);
		void ResizeMainWindow(); // To what's shown

	private:
		RecentProjects _recentProjects;
		String         _openError; // The last project that couldn't be opened

		bool           _newProjectFormShown = false; // In place of the project list
		uint16_t       _mainWindowHeight = 320; // As Editor::Init sets it
		NewProjectForm _newProjectForm;
	};
}
