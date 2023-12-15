#pragma once
#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window.hpp>

#include <vector>
#include <filesystem>

#include "HodEngine/Editor/RecentProjects.hpp"

namespace hod::editor
{
	/// @brief 
	class ProjectBrowser : public imgui::Window
	{
		META_TYPE(ProjectBrowser, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					ProjectBrowser();
					~ProjectBrowser() override = default;

	public:

		void		Draw() override;

	private:

		RecentProjects _recentProjects;
	};
}
