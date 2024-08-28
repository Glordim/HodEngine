#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/ImGui/Window/Popup.hpp" 
#include "HodEngine/ImGui/DearImGui/imgui.h"

namespace hod::imgui
{
	/// @brief 
	Popup::Popup()
	{
		//ImGui::OpenPopup(GetIdentifier());
	}

	/// @brief 
	/// @return 
	bool Popup::Draw()
	{
		ImGui::OpenPopup(GetIdentifier());
		bool open = true;
		if (ImGui::BeginPopup(GetIdentifier()) == true)
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
