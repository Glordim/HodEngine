#include "MainBar.h"

#include <HodEngine/ImGui/src/DearImGui/imgui.h>
#include <HodEngine/Application/src/Application.h>

namespace hod::editor
{
	/// @brief 
	void MainBar::Draw()
	{
		if (ImGui::BeginMenu("File") == true)
		{
			if (ImGui::MenuItem("Quit") == true)
			{
				application::Application::GetInstance()->Quit();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit") == true)
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window") == true)
		{
			if (ImGui::MenuItem("Asset Browser") == true)
			{

			}
			ImGui::EndMenu();
		}
	}
}
