#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Editor/RecentProjects.hpp"

namespace hod::inline editor
{
	/// @brief
	class HOD_EDITOR_API ProjectBrowser : public Window
	{
		REFLECTED_CLASS(ProjectBrowser, Window)

	public:
		ProjectBrowser();
		~ProjectBrowser() override = default;

	public:
		void DrawContent() override;

	private:
		RecentProjects _recentProjects;
	};
}
