#include "HodEngine/Editor/MainBar.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/Application/Application.hpp>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/Application/PlatformDialog.hpp>

namespace hod::editor
{
	/// @brief 
	void MainBar::Draw()
	{
		if (ImGui::BeginMenu("File") == true)
		{
			if (ImGui::MenuItem("Save", "CTRL+S") == true)
			{
				Editor::GetInstance()->Save();
			}

			if (ImGui::MenuItem("Save scene As", "CTRL+S") == true)
			{
				Editor::GetInstance()->SaveSceneAs();
			}

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
