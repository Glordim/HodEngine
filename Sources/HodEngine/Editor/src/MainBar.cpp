#include "MainBar.h"

#include <HodEngine/ImGui/src/DearImGui/imgui.h>
#include <HodEngine/Application/src/Application.h>

#include <HodEngine/ImGui/src/ImGuiManager.h>

#include "HodEngine/Editor/src/AssetBrowserWindow.h"
#include "HodEngine/Editor/src/InspectorWindow.h"
#include "HodEngine/Editor/src/ViewportWindow.h"
#include "HodEngine/Editor/src/HierachyWindow.h"
#include "HodEngine/Editor/src/Editor.h"

#include <HodEngine/Application/src/PlatformDialog.h>

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
				imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
			}
			if (ImGui::MenuItem("Inspector") == true)
			{
				imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
			}
			if (ImGui::MenuItem("Viewport") == true)
			{
				imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>();
			}
			if (ImGui::MenuItem("Hierachy") == true)
			{
				imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
			}
			ImGui::EndMenu();
		}
	}
}
