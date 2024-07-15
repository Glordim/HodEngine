#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window/Window.hpp>

#include <vector>
#include <filesystem>

#include "HodEngine/Editor/RecentProjects.hpp"

namespace hod::editor
{
	/// @brief 
	class HOD_API ProjectBrowser : public imgui::Window
	{
		META_TYPE(ProjectBrowser, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					ProjectBrowser();
					~ProjectBrowser() override = default;

	public:

		void		DrawContent() override;

	private:

		RecentProjects _recentProjects;
	};
}
