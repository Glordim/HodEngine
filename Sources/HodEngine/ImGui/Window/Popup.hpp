#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "HodEngine/ImGui/Window/Window.hpp"

namespace hod::imgui
{
	/// @brief 
	class HOD_IMGUI_API Popup : public Window
	{
		REFLECTED_CLASS(Popup, Window)

	public:

				Popup();
				~Popup() override = default;

	public:

		bool	Draw() override;
	};
}
