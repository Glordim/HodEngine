#include "HodEngine/Editor/InspectorWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Editor/AssetBrowserWindow.h"
#include "HodEngine/Editor/HierachyWindow.h"

namespace hod::editor
{
	/// @brief 
	void InspectorWindow::Draw()
	{
		imgui::Window* activeWindow = imgui::ImGuiManager::GetInstance()->GetActiveWindow();
		if (activeWindow != nullptr)
		{
			if (activeWindow->GetType() == AssetBrowserWindow::_type)
			{
				ImGui::Text("Asset");
			}
			else if (activeWindow->GetType() == HierachyWindow::_type)
			{
				ImGui::Text("Scene");
			}
		}
	}
}
