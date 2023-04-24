#pragma once
#include "AssetDatabase.h"
#include <HodEngine/ImGui/src/Window.h>

#include <vector>
#include <filesystem>

#include "RecentProjects.h"

namespace hod::editor
{
	/// @brief 
	class ProjectBrowser : public imgui::Window
	{
	public:

					ProjectBrowser();
					~ProjectBrowser() override = default;

	public:

		void		Draw() override;

	private:

		RecentProjects _recentProjects;
		std::vector<std::filesystem::path>	_projectsPath;
	};
}
