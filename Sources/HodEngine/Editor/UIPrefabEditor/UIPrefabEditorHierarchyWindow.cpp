#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorHierarchyWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorHierarchyWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	UIPrefabEditorHierarchyWindow::UIPrefabEditorHierarchyWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Hierarchy");
	}

	/// @brief
	void UIPrefabEditorHierarchyWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		DrawNode(tab->GetCanvas().GetRootNode());
	}

	/// @brief
	/// @param node
	void UIPrefabEditorHierarchyWindow::DrawNode(ui2::Node* node)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		ui2::Node*         selection = tab->GetSelectedNode();

		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow;
		if (node->GetChildren().Empty())
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (node == selection)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(node);
		bool opened = ImGui::TreeNodeEx(node->GetParent() == nullptr ? "Root" : "Node", treeNodeFlags);
		if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen() == false)
		{
			tab->SetSelectedNode(node);
		}

		bool hovered = ImGui::IsItemHovered();
		if (ImGui::IsWindowHovered() && hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			tab->SetSelectedNode(node);
			ImGui::OpenPopup("UIPrefabNodeContextMenu");
		}

		bool deleted = false;
		if (ImGui::BeginPopup("UIPrefabNodeContextMenu"))
		{
			if (ImGui::MenuItem(ICON_MDI_PLUS_BOX " Add Child"))
			{
				node->AddChild(DefaultAllocator::GetInstance().New<ui2::Node>());
				tab->MarkAsDirty();
			}
			if (node->GetParent() != nullptr && ImGui::MenuItem(ICON_MDI_DELETE " Delete"))
			{
				ui2::Node* parent = node->GetParent();
				if (tab->GetSelectedNode() == node)
				{
					tab->SetSelectedNode(nullptr);
				}
				parent->RemoveChild(node);
				DefaultAllocator::GetInstance().Delete(node);
				tab->MarkAsDirty();
				deleted = true;
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			if (deleted == false)
			{
				for (ui2::Node* child : node->GetChildren())
				{
					DrawNode(child);
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}
