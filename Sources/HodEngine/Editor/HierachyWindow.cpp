#include "HodEngine/Editor/HierachyWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

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