#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/SceneEditor/SceneEditorTab.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

namespace hod::editor
{
	/// @brief 
	SceneEditorTab::SceneEditorTab()
	: EditorTab(ICON_MDI_IMAGE_FILTER_HDR "   Untitled")
	{
	}

	/// @brief 
	void SceneEditorTab::CreateDefaultLayout()
	{
		//imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
		HierachyWindow* hierarchyWindow = imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
		InspectorWindow* inspectorWindow = imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
		ViewportWindow* viewportWindow = imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGuiID dockCentral = ImGui::DockBuilderAddNode(_dockSpaceId, ImGuiDockNodeFlags_CentralNode);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Left, 0.2f, &dockCentral, &opposite);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Right, 0.2f, nullptr, nullptr);

		ImGui::DockBuilderDockWindow(hierarchyWindow->GetIdentifier(), dockLeft);
		ImGui::DockBuilderDockWindow(inspectorWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), dockCentral);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief 
	/// @return 
	bool SceneEditorTab::DrawContent()
	{
		return true;
	}
}
