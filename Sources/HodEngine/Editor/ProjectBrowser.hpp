#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <filesystem>

#include "HodEngine/Editor/RecentProjects.hpp"

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API ProjectBrowser : public imgui::Window
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
