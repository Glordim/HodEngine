#pragma once

#include <HodEngine/ImGui/Window.hpp>
#include <HodEngine/Core/Math/Matrix4.hpp>

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

		Matrix4					_view;
		float					_size = 5.0f;
		renderer::RenderTarget* _renderTarget = nullptr;
	};
}
