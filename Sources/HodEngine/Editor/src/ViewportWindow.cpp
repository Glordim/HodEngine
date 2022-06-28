#include "ViewportWindow.h"

#include <HodEngine/ImGui/src/DearImGui/imgui.h>

#include <HodEngine/ImGui/src/ImGuiManager.h>

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
