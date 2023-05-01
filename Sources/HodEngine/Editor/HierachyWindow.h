#pragma once

#include <HodEngine/ImGui/Window.h>

namespace hod::editor
{
	/// @brief 
	class HierachyWindow : public imgui::Window
	{
	public:

					HierachyWindow() = default;
					~HierachyWindow() override = default;

	public:

		void		Draw() override;
	};
}
