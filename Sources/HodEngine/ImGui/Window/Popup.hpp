#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/Type.hpp"
#include "HodEngine/ImGui/Window/Window.hpp"

namespace hod::imgui
{
	/// @brief 
	class HOD_API Popup : public Window
	{
		META_TYPE(Popup, Window);

	public:

				Popup();
				~Popup() override = default;

	public:

		bool	Draw() override;
	};
}
