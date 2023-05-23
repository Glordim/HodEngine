#pragma once
#include "HodEngine/Editor/AssetDatabase.h"
#include <HodEngine/ImGui/Window.h>

#include <vector>
#include <filesystem>

#include "HodEngine/Editor/RecentProjects.h"

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
	};
}
