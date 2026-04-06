#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "HodEngine/ImGui/Window/Popup.hpp"

namespace hod::imgui
{
	/// @brief 
	class HOD_IMGUI_API Modal : public Popup
	{
		REFLECTED_CLASS(Modal, Popup)

	public:

				Modal() = default;
				~Modal() override = default;

	public:

		bool	Draw() override;
	};
}
