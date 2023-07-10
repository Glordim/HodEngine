#pragma once

#include <HodEngine/ImGui/Window.h>

namespace hod::renderer
{
	class RenderTarget;
}

namespace hod::editor
{
	/// @brief 
	class ViewportWindow : public imgui::Window
	{
		META_TYPE(ViewportWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					ViewportWindow();
					~ViewportWindow() override;

	public:

		void		Draw() override;

	private:

		renderer::RenderTarget* _renderTarget = nullptr;
	};
}
