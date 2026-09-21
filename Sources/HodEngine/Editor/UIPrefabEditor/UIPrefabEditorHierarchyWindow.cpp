#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorHierarchyWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>
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

		// A node saved before names existed has none: fall back on its type rather than an empty label.
		const String& label = node->GetName().Empty() ? node->GetReflectionDescriptorV().GetDisplayName() : node->GetName();

		// Ids identify the node, names may collide between siblings: the ImGui id is the node itself.
		ImGui::PushID(node);
		bool opened = ImGui::TreeNodeEx(label.CStr(), treeNodeFlags);
		if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen() == false)
		{
			tab->SetSelectedNode(node);
		}

		// Dropping a node from the Library adds it as a child of the hovered node.
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UI2NodeLibraryDescriptor");
			if (payload != nullptr)
			{
				ReflectionDescriptor* nodeDescriptor = *static_cast<ReflectionDescriptor**>(payload->Data);

				ui2::Node* newNode = ui2::NodeFactory::GetInstance()->CreateNode(*nodeDescriptor);
				node->AddChild(newNode);

				// Keep the new child visible: a previously leaf/collapsed target would otherwise hide it.
				ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), 1);

				tab->SetSelectedNode(newNode);
				tab->MarkAsDirty();
			}
			ImGui::EndDragDropTarget();
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
				node->AddChild(ui2::NodeFactory::GetInstance()->CreateNode(ui2::Node::GetReflectionDescriptor()));
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
