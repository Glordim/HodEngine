#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/ImGui/Window/Modal.hpp"
#include "HodEngine/ImGui/DearImGui/imgui.h"

namespace hod::inline imgui
{
	DESCRIBE_REFLECTED_CLASS(Modal, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @return 
	bool Modal::Draw()
	{
		ImGui::OpenPopup(GetIdentifier());
		bool open = true;
		if (ImGui::BeginPopupModal(GetIdentifier(), &open) == true)
		{
			DrawContent();
			ImGui::EndPopup();
		}
		if (open == false)
		{
			Close();
		}
		return open;
	}
}
