#pragma once

#include <HodEngine/ImGui/Window.h>

namespace hod::editor
{
	/// @brief 
	class InspectorWindow : public imgui::Window
	{
		META_TYPE(InspectorWindow);

	public:

					InspectorWindow() = default;
					~InspectorWindow() override = default;

	public:

		void		Draw() override;
	};
}
