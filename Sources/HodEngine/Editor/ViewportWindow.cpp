#include "HodEngine/Editor/ViewportWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

namespace hod::editor
{
	/// @brief 
	void ViewportWindow::Draw()
	{
		static bool open = true;
		if (ImGui::Begin("Viewport", &open) == true)
		{

		}
		ImGui::End();
	}
}
