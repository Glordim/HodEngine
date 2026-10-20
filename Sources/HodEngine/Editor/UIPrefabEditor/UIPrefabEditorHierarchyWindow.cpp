#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorHierarchyWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"

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
		// A newly selected node may sit under collapsed parents (picked in the viewport): for that one
		// frame the ancestors are forced open and the node scrolled into view. Only on a change, so the
		// user stays free to collapse them again afterwards.
		ui2::Node* selection = tab->GetSelectedNode();
		_revealSelection = (selection != nullptr && selection != _lastSelectedNode);
		_lastSelectedNode = selection;

		// The canvas stands at the top, greyed out (not a node of the prefab: it cannot be moved, deleted or
		// given children), so its settings are selected like any other item. The canvas' own root node is
		// only the hidden container of the prefab root, it is never shown.
		ImGuiTreeNodeFlags canvasFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
		if (tab->IsCanvasSelected())
		{
			canvasFlags |= ImGuiTreeNodeFlags_Selected;
		}
		if (_revealSelection)
		{
			ImGui::SetNextItemOpen(true);
		}
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		bool canvasOpened = ImGui::TreeNodeEx("##Canvas", canvasFlags, ICON_MDI_MONITOR_SCREENSHOT " Canvas");
		ImGui::PopStyleColor();
		if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen() == false)
		{
			tab->SelectCanvas();
		}

		if (canvasOpened)
		{
			DrawNode(tab->GetPrefabRoot());
			ImGui::TreePop();
		}

		if (_pendingReparentNode != nullptr)
		{
			_pendingReparentNode->GetParent()->ReparentChild(_pendingReparentNode, _pendingReparentParent);
			_pendingReparentNode = nullptr;
			_pendingReparentParent = nullptr;
			tab->MarkAsDirty();
		}

		if (_pendingConvertNode != nullptr)
		{
			tab->ConvertNode(_pendingConvertNode, *_pendingConvertDescriptor);
			_pendingConvertNode = nullptr;
			_pendingConvertDescriptor = nullptr;
		}
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
		if (_revealSelection)
		{
			for (ui2::Node* ancestor = selection->GetParent(); ancestor != nullptr; ancestor = ancestor->GetParent())
			{
				if (ancestor == node)
				{
					ImGui::SetNextItemOpen(true);
					break;
				}
			}
		}
		const char* icon = DrawHelper::GetTypeIcon(node->GetReflectionDescriptorV());
		bool        opened = ImGui::TreeNodeEx("##Node", treeNodeFlags, "%s %s", icon, label.CStr());
		if (_revealSelection && node == selection)
		{
			ImGui::SetScrollHereY();
		}
		if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen() == false)
		{
			tab->SetSelectedNode(node);
		}

		// Any node can be dragged onto another to become its child, except the prefab root: it has no
		// place to go, being the one and only child of the hidden container.
		if (node != tab->GetPrefabRoot() && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("UI2Node", &node, sizeof(ui2::Node*));
			ImGui::Text("%s %s", icon, label.CStr());
			ImGui::EndDragDropSource();
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

			// Only offered (highlighted, then accepted) when the move makes sense: not onto itself, onto
			// one of its own descendants, or onto the parent it already has.
			const ImGuiPayload* peekedPayload = ImGui::GetDragDropPayload();
			if (peekedPayload != nullptr && peekedPayload->IsDataType("UI2Node"))
			{
				ui2::Node* draggedNode = *static_cast<ui2::Node**>(peekedPayload->Data);
				if (draggedNode != node && node->IsDescendantOf(draggedNode) == false && draggedNode->GetParent() != node)
				{
					payload = ImGui::AcceptDragDropPayload("UI2Node");
					if (payload != nullptr)
					{
						_pendingReparentNode = draggedNode;
						_pendingReparentParent = node;

						// Keep the moved node visible: a previously leaf/collapsed target would otherwise hide it.
						ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), 1);
					}
				}
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
			if (ImGui::BeginMenu(ICON_MDI_SWAP_HORIZONTAL " Convert To"))
			{
				uint64_t currentType = node->GetReflectionDescriptorV().GetType();
				for (const auto& descriptorPair : ui2::NodeFactory::GetInstance()->GetAllDescriptors())
				{
					bool isCurrent = (descriptorPair.second->GetType() == currentType);
					String typeLabel = String(DrawHelper::GetTypeIcon(*descriptorPair.second)) + " " + descriptorPair.second->GetDisplayName();
					if (ImGui::MenuItem(typeLabel.CStr(), nullptr, isCurrent, isCurrent == false))
					{
						_pendingConvertNode = node;
						_pendingConvertDescriptor = descriptorPair.second;
					}
				}
				ImGui::EndMenu();
			}
			if (node != tab->GetPrefabRoot() && ImGui::MenuItem(ICON_MDI_DELETE " Delete"))
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
