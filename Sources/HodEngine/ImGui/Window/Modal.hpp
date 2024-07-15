#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/Type.hpp"
#include "HodEngine/ImGui/Window/Popup.hpp"

namespace hod::imgui
{
	/// @brief 
	class HOD_API Modal : public Popup
	{
		META_TYPE(Modal, Popup);

	public:

				Modal() = default;
				~Modal() override = default;

	public:

		bool	Draw() override;
	};
}
