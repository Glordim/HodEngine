#pragma once
#include "AssetDatabase.h"
#include <HodEngine/ImGui/src/Window.h>

#include <vector>
#include <filesystem>

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
	};
}
