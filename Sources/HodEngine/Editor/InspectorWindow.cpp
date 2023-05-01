#include "HodEngine/Editor/InspectorWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

namespace hod::editor
{
	/// @brief 
	void InspectorWindow::Draw()
	{
		static bool open = true;
		if (ImGui::Begin("Inspector", &open) == true)
		{

		}
		ImGui::End();
	}
}
