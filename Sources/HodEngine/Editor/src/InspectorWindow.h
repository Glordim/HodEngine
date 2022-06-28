#pragma once

#include <HodEngine/ImGui/src/Window.h>

namespace hod::editor
{
	/// @brief 
	class InspectorWindow : public imgui::Window
	{
	public:

					InspectorWindow() = default;
					~InspectorWindow() override = default;

	public:

		void		Draw() override;
	};
}
