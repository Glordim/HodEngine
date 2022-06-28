#include "HierachyWindow.h"

#include <HodEngine/ImGui/src/DearImGui/imgui.h>

#include <HodEngine/ImGui/src/ImGuiManager.h>

namespace hod::editor
{
	/// @brief 
	void HierachyWindow::Draw()
	{
		static bool open = true;
		if (ImGui::Begin("Hierachy", &open) == true)
		{

		}
		ImGui::End();
	}
}
