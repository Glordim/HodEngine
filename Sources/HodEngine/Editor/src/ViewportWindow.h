#pragma once

#include <HodEngine/ImGui/src/Window.h>

namespace hod::editor
{
	/// @brief 
	class ViewportWindow : public imgui::Window
	{
	public:

					ViewportWindow() = default;
					~ViewportWindow() override = default;

	public:

		void		Draw() override;
	};
}
