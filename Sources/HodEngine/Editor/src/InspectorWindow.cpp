#include "InspectorWindow.h"

#include <HodEngine/ImGui/src/DearImGui/imgui.h>

#include <HodEngine/ImGui/src/ImGuiManager.h>

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
