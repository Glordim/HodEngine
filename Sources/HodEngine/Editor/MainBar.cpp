#include "HodEngine/Editor/MainBar.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/Application/Application.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Editor/AssetBrowserWindow.h"
#include "HodEngine/Editor/InspectorWindow.h"
#include "HodEngine/Editor/ViewportWindow.h"
#include "HodEngine/Editor/HierachyWindow.h"
#include "HodEngine/Editor/Editor.h"

#include <HodEngine/Application/PlatformDialog.h>

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
