#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/UI2/Node.hpp>

#include <algorithm>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorViewportWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	UIPrefabEditorViewportWindow::UIPrefabEditorViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Viewport");
	}

	/// @brief
	void UIPrefabEditorViewportWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		ui2::Node*         root = tab->GetCanvas().GetRootNode();

		Vector2 canvasSize = root->ComputeSize();
		ImVec2  available = ImGui::GetContentRegionAvail();
		if (available.x <= 1.0f || available.y <= 1.0f || canvasSize.GetX() <= 0.0f || canvasSize.GetY() <= 0.0f)
		{
			return;
		}

		float  scale = std::min(available.x / canvasSize.GetX(), available.y / canvasSize.GetY());
		ImVec2 canvasScreenSize(canvasSize.GetX() * scale, canvasSize.GetY() * scale);

		ImVec2 cursor = ImGui::GetCursorScreenPos();
		// origin: screen position of canvas-space (0, 0), i.e. the center of the design-resolution rect
		ImVec2 origin(cursor.x + available.x * 0.5f, cursor.y + available.y * 0.5f);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2      canvasMin(origin.x - canvasScreenSize.x * 0.5f, origin.y - canvasScreenSize.y * 0.5f);
		ImVec2      canvasMax(origin.x + canvasScreenSize.x * 0.5f, origin.y + canvasScreenSize.y * 0.5f);
		drawList->AddRectFilled(canvasMin, canvasMax, IM_COL32(30, 30, 30, 255));
		drawList->AddRect(canvasMin, canvasMax, IM_COL32(90, 90, 90, 255));

		DrawNode(root, origin, scale);

		ImGui::Dummy(available);
	}

	/// @brief
	/// @param node
	/// @param origin
	/// @param scale
	void UIPrefabEditorViewportWindow::DrawNode(ui2::Node* node, const ImVec2& origin, float scale)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		Vector2 size = node->ComputeSize();
		Vector2 canvasPosition = node->ComputeCanvasMatrix().GetTranslation();

		// Canvas space is assumed Y-up (RectTransform-style); screen space is Y-down, hence the flip.
		ImVec2 center(origin.x + canvasPosition.GetX() * scale, origin.y - canvasPosition.GetY() * scale);
		ImVec2 halfExtent(size.GetX() * 0.5f * scale, size.GetY() * 0.5f * scale);

		ImVec2 rectMin(center.x - halfExtent.x, center.y - halfExtent.y);
		ImVec2 rectMax(center.x + halfExtent.x, center.y + halfExtent.y);

		bool        isSelected = (node == tab->GetSelectedNode());
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRect(rectMin, rectMax, isSelected ? IM_COL32(255, 165, 0, 255) : IM_COL32(120, 170, 255, 180), 0.0f, isSelected ? 2.0f : 1.0f);

		if (ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(rectMin, rectMax) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			tab->SetSelectedNode(node);
		}

		for (ui2::Node* child : node->GetChildren())
		{
			DrawNode(child, origin, scale);
		}
	}
}
