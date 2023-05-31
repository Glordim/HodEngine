#pragma once

#include <HodEngine/ImGui/Window.h>

namespace hod::editor
{
	/// @brief 
	class ViewportWindow : public imgui::Window
	{
		META_TYPE(ViewportWindow);

	public:

					ViewportWindow() = default;
					~ViewportWindow() override = default;

	public:

		void		Draw() override;
	};
}
