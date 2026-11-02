#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorHierarchyWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>
#include <HodEngine/Core/Hash.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

#include <algorithm>

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
		if (tab->IsPlaying())
		{
			ImGui::TextDisabled("Editing suspended while playing");
			return;
		}

		// Instances rebuilt from their saved prefab: their nodes are gone
		if (tab->GetRebuildCount() != _rebuildCount)
		{
			_rebuildCount = tab->GetRebuildCount();
			_renamedNode = nullptr;
			_lastSelectedNode = nullptr;
		}

		// A newly active node may sit under collapsed parents (picked in the viewport): for that one frame the ancestors are
		// forced open and the node scrolled into view. Only on a change, so the user stays free to collapse them again
		// afterwards. Not for the selections made here (see the end of the multi-selection scope), of nodes already in view.
		ui2::Node* selection = tab->GetSelectedNode();
		_revealSelection = (selection != nullptr && selection != _lastSelectedNode);
		_lastSelectedNode = selection;

		// F2 renames the selected node, as in the explorer.
		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_F2) && ImGui::GetIO().WantTextInput == false && selection != nullptr && _renamedNode == nullptr &&
		    UIPrefabEditorTab::CanRename(selection))
		{
			StartRename(selection);
		}

		// F frames the selection in the viewport.
		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_F, false) && ImGui::GetIO().WantTextInput == false)
		{
			tab->RequestFrameSelection();
		}

		// Delete removes the selected nodes (except the prefab root and the instances' content).
		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete, false) && ImGui::GetIO().WantTextInput == false)
		{
			_pendingDeleteNodes = tab->GetSelectedNodes();
		}
		_renameSubmitted = false;
		bool renamingAtStart = (_renamedNode != nullptr);

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
			// The nodes only: the canvas, exclusive with them, is selected by its own click above. Escape is for a pending cut first.
			ImGuiMultiSelectFlags multiSelectFlags = ImGuiMultiSelectFlags_BoxSelect1d;
			if (tab->HasCutNodes() == false)
			{
				multiSelectFlags |= ImGuiMultiSelectFlags_ClearOnEscape;
			}
			// ImGui sees a click in a context menu as a click on the void of the scope (the popup is in this window hierarchy): it
			// would clear the selection the menu acts on (see AssetBrowserWindow)
			if (ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId) == false)
			{
				multiSelectFlags |= ImGuiMultiSelectFlags_ClearOnClickVoid;
			}
			// The multi-selection always draws ImGui's nav cursor (the keyboard focus outline), which any click moves to the clicked
			// node, its arrow included: it would look like the selection of a node only opened. Only shown while navigating with
			// the keyboard, as outside of a multi-selection.
			bool hideNavCursor = (ImGui::GetCurrentContext()->NavCursorVisible == false);
			if (hideNavCursor)
			{
				ImGui::PushStyleColor(ImGuiCol_NavCursor, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			}
			ImGuiMultiSelectIO* multiSelectIO = ImGui::BeginMultiSelect(multiSelectFlags, static_cast<int>(tab->GetSelectedNodes().Size()), -1);
			ApplySelectionRequests(multiSelectIO, false);

			_visibleNodes.Clear();
			DrawNode(tab->GetPrefabRoot());

			multiSelectIO = ImGui::EndMultiSelect();
			if (hideNavCursor)
			{
				ImGui::PopStyleColor();
			}
			ApplySelectionRequests(multiSelectIO, true);
			_lastSelectedNode = tab->GetSelectedNode(); // made here: no reveal
			ImGui::TreePop();
		}

		// The rename field was not drawn this frame (a parent got collapsed...): it can't lose the focus anymore, so
		// validate it now. Unless the rename has just been started and its field is yet to be drawn.
		if (_renamedNode != nullptr && _renameSubmitted == false && _renameFocus == false)
		{
			ApplyRename();
			_renamedNode = nullptr;
		}

		if (_pendingReparentNodes.Empty() == false)
		{
			tab->ReparentNodes(_pendingReparentNodes, _pendingReparentParent);
			_pendingReparentNodes.Clear();
			_pendingReparentParent = nullptr;
		}

		if (_pendingConvertNode != nullptr)
		{
			if (_renamedNode == _pendingConvertNode)
			{
				_renamedNode = nullptr; // Deleted by the conversion
			}
			tab->ConvertNode(_pendingConvertNode, *_pendingConvertDescriptor);
			_pendingConvertNode = nullptr;
			_pendingConvertDescriptor = nullptr;
		}

		if (_pendingDeleteNodes.Empty() == false)
		{
			for (ui2::Node* deletedNode : _pendingDeleteNodes)
			{
				if (_renamedNode != nullptr && tab->CanRemove(deletedNode) && (_renamedNode == deletedNode || _renamedNode->IsDescendantOf(deletedNode)))
				{
					_renamedNode = nullptr;
				}
			}
			tab->DeleteNodes(_pendingDeleteNodes);
			_pendingDeleteNodes.Clear();
		}

		if (_pendingPaste)
		{
			if (_pendingPasteParent != nullptr)
			{
				tab->PasteNodes(_pendingPasteParent);
			}
			else
			{
				tab->PasteNodesNextToSelection();
			}
			_pendingPaste = false;
			_pendingPasteParent = nullptr;
		}

		if (_pendingDuplicateNodes.Empty() == false)
		{
			tab->DuplicateNodes(_pendingDuplicateNodes);
			_pendingDuplicateNodes.Clear();
		}

		if (_selectAllRequested)
		{
			tab->SetSelectedNodes(_visibleNodes);
			_lastSelectedNode = tab->GetSelectedNode(); // made here: no reveal
			_selectAllRequested = false;
		}

		// Ctrl+C / Ctrl+X / Ctrl+V / Ctrl+Shift+V / Ctrl+D / Escape, as in the viewport. After the tree has been drawn, for the same reason as above.
		// Not while renaming, nor on the frame a rename ends: its Escape isn't for the cut.
		if (ImGui::IsWindowFocused() && ImGui::GetIO().WantTextInput == false && renamingAtStart == false && _renamedNode == nullptr)
		{
			tab->HandleClipboardShortcuts();
		}
	}

	/// @brief
	/// @param multiSelectIO
	/// @param scopeEnd
	void UIPrefabEditorHierarchyWindow::ApplySelectionRequests(ImGuiMultiSelectIO* multiSelectIO, bool scopeEnd)
	{
		// The nav init of a newly focused window puts the nav on the first item, and the multi-selection auto-selects it as if it
		// was reached with the arrows: only a real move selects (see AssetBrowserWindow)
		const ImGuiContext& g = *ImGui::GetCurrentContext();
		if (g.NavJustMovedToId != 0 && g.NavJustMovedToIsInit)
		{
			return;
		}

		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		for (const ImGuiSelectionRequest& request : multiSelectIO->Requests)
		{
			if (request.Type == ImGuiSelectionRequestType_SetAll)
			{
				if (request.Selected == false)
				{
					tab->SetSelectedNode(nullptr);
				}
				else if (scopeEnd)
				{
					tab->SetSelectedNodes(_visibleNodes);
				}
				else
				{
					_selectAllRequested = true; // _visibleNodes is last frame's, its nodes may be gone
				}
			}
			else if (request.Type == ImGuiSelectionRequestType_SetRange && scopeEnd)
			{
				// From the range's first item to its last one, so that the last one (the clicked one) ends up the active one. An end
				// no longer drawn (collapsed since) leaves the other one only.
				ui2::Node* firstNode = reinterpret_cast<ui2::Node*>(static_cast<intptr_t>(request.RangeFirstItem));
				ui2::Node* lastNode = reinterpret_cast<ui2::Node*>(static_cast<intptr_t>(request.RangeLastItem));
				auto       firstIt = std::find(_visibleNodes.Begin(), _visibleNodes.End(), firstNode);
				auto       lastIt = std::find(_visibleNodes.Begin(), _visibleNodes.End(), lastNode);
				if (firstIt == _visibleNodes.End())
				{
					firstIt = lastIt;
				}
				else if (lastIt == _visibleNodes.End())
				{
					lastIt = firstIt;
				}
				if (firstIt == _visibleNodes.End())
				{
					continue;
				}

				int32_t first = static_cast<int32_t>(firstIt - _visibleNodes.Begin());
				int32_t last = static_cast<int32_t>(lastIt - _visibleNodes.Begin());
				int32_t step = (first <= last) ? 1 : -1;
				for (int32_t index = first;; index += step)
				{
					tab->SetNodeSelected(_visibleNodes[static_cast<uint32_t>(index)], request.Selected);
					if (index == last)
					{
						break;
					}
				}
			}
		}
	}

	/// @brief Replaces node's label by a text field, focused on its next draw.
	/// @param node
	void UIPrefabEditorHierarchyWindow::StartRename(ui2::Node* node)
	{
		_renamedNode = node;
		std::strncpy(_renameBuffer, node->GetName().CStr(), sizeof(_renameBuffer) - 1);
		_renameBuffer[sizeof(_renameBuffer) - 1] = '\0';
		_renameFocus = true;
	}

	/// @brief Gives the renamed node the name typed in its field.
	void UIPrefabEditorHierarchyWindow::ApplyRename()
	{
		if (_renamedNode != nullptr && _renamedNode->GetName() != _renameBuffer)
		{
			_renamedNode->SetName(_renameBuffer);
			GetOwner()->MarkAsDirty("Rename");
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
		if (tab->IsSelected(node))
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
		// Nested prefabs: an instance's root has the prefab icon, the instances' content is drawn in blue (paler where read-only)
		UIPrefabEditorTab::NodeRole role = UIPrefabEditorTab::GetNodeRole(node);
		ui2::PrefabInstance*        prefabInstance = node->GetPrefabInstance();
		bool                        canEditStructure = (role == UIPrefabEditorTab::NodeRole::Authored);
		const char*                 icon = DrawHelper::GetTypeIcon(node->GetReflectionDescriptorV());
		if (prefabInstance != nullptr)
		{
			icon = prefabInstance->IsMissing() ? ICON_MDI_ALERT : ICON_MDI_PACKAGE_VARIANT_CLOSED;
		}
		String displayedLabel = label;
		if (prefabInstance != nullptr && prefabInstance->IsMissing())
		{
			displayedLabel = "Missing Prefab";
		}

		bool  renaming = (node == _renamedNode);
		float treeNodeStartX = ImGui::GetCursorPosX();
		// While renamed, only the icon is drawn by the tree node: the rename field takes the label's place (see below).
		// A cut node (and its subtree) is greyed out until pasted, as in a file explorer
		bool  pushedColor = true;
		if (tab->IsCut(node))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		}
		else if (prefabInstance != nullptr && prefabInstance->IsMissing())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
		}
		else if (role == UIPrefabEditorTab::NodeRole::ReadOnly)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.75f, 1.0f, 0.5f));
		}
		else if (role != UIPrefabEditorTab::NodeRole::Authored)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.75f, 1.0f, 1.0f));
		}
		else
		{
			pushedColor = false;
		}
		ImGui::SetNextItemSelectionUserData(static_cast<ImGuiSelectionUserData>(reinterpret_cast<intptr_t>(node)));
		_visibleNodes.PushBack(node);
		bool opened = renaming ? ImGui::TreeNodeEx("##Node", treeNodeFlags, "%s", icon) : ImGui::TreeNodeEx("##Node", treeNodeFlags, "%s %s", icon, displayedLabel.CStr());
		// Kept to open the tree node from its context menu, where the current item and storage are the popup's
		ImGuiID       treeNodeId = ImGui::GetItemID();
		ImGuiStorage* treeNodeStorage = ImGui::GetStateStorage();
		if (pushedColor)
		{
			ImGui::PopStyleColor();
		}
		if (prefabInstance != nullptr)
		{
			std::shared_ptr<Asset> prefabAsset = AssetDatabase::GetInstance()->Find(prefabInstance->GetPrefab());
			ImGui::SetItemTooltip("Instance of %s%s", prefabAsset != nullptr ? prefabAsset->GetName().CStr() : prefabInstance->GetPrefab().ToString().CStr(),
			                      prefabInstance->IsMissing() ? " (can't be loaded)" : "");
		}
		else if (role == UIPrefabEditorTab::NodeRole::ReadOnly)
		{
			ImGui::SetItemTooltip("In an instance nested in an instance: edited in its own prefab");
		}
		if (_revealSelection && node == selection)
		{
			ImGui::SetScrollHereY();
		}
		// Clicks are the multi-selection's (see ApplySelectionRequests)

		// Any node can be dragged onto another to become its child, except the prefab root: it has no
		// place to go, being the one and only child of the hidden container. Nor an instance's content, which follows its prefab.
		// The selection goes along when the node is in it. The payload is the dragged node only, as other targets take one node
		// (a NodeRef, the asset browser).
		if (tab->CanRemove(node) && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("UI2Node", &node, sizeof(ui2::Node*));
			uint32_t draggedCount = tab->IsSelected(node) ? tab->GetSelectedSubtrees().Size() : 1;
			if (draggedCount > 1)
			{
				ImGui::Text("%s %s (+%u)", icon, displayedLabel.CStr(), draggedCount - 1);
			}
			else
			{
				ImGui::Text("%s %s", icon, displayedLabel.CStr());
			}
			ImGui::EndDragDropSource();
		}

		// Dropping a node from the Library adds it as a child of the hovered node, a UI prefab from the asset browser an instance of it.
		// Not into an instance: its structure is its prefab's.
		if (canEditStructure && ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* assetPayload = ImGui::GetDragDropPayload();
			if (assetPayload != nullptr && assetPayload->IsDataType("FileSystemMapping"))
			{
				AssetDatabase::FileSystemMapping* mapping = *static_cast<AssetDatabase::FileSystemMapping**>(assetPayload->Data);
				if (mapping->_asset != nullptr && mapping->_asset->GetType() == Hash::ComputeXxh3_64("UIPrefab") && ImGui::AcceptDragDropPayload("FileSystemMapping") != nullptr)
				{
					if (tab->InstantiatePrefab(mapping->_asset->GetUid(), node) != nullptr)
					{
						ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), 1);
					}
				}
			}

			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UI2NodeLibraryDescriptor");
			if (payload != nullptr)
			{
				ReflectionDescriptor* nodeDescriptor = *static_cast<ReflectionDescriptor**>(payload->Data);

				ui2::Node* newNode = ui2::NodeFactory::GetInstance()->CreateNode(*nodeDescriptor);
				node->AddChild(newNode);

				// Keep the new child visible: a previously leaf/collapsed target would otherwise hide it.
				ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), 1);

				tab->SetSelectedNode(newNode);
				tab->MarkAsDirty(fmt::format("Add {}", nodeDescriptor->GetDisplayName()));
			}

			// Only offered (highlighted, then accepted) when the move makes sense: not onto one of the dragged nodes, into one, or
			// onto the parent they all already have (see UIPrefabEditorTab::CanReparentNodes).
			const ImGuiPayload* peekedPayload = ImGui::GetDragDropPayload();
			if (peekedPayload != nullptr && peekedPayload->IsDataType("UI2Node"))
			{
				ui2::Node*         draggedNode = *static_cast<ui2::Node**>(peekedPayload->Data);
				Vector<ui2::Node*> draggedNodes = tab->IsSelected(draggedNode) ? tab->GetSelectedNodes() : Vector<ui2::Node*>{draggedNode};
				if (tab->CanReparentNodes(draggedNodes, node))
				{
					payload = ImGui::AcceptDragDropPayload("UI2Node");
					if (payload != nullptr)
					{
						_pendingReparentNodes = draggedNodes;
						_pendingReparentParent = node;

						// Keep the moved node visible: a previously leaf/collapsed target would otherwise hide it.
						ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), 1);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// The right click selects the node if it isn't (the multi-selection's, on mouse down): the menu acts on the selection, the
		// node made the active one (the one the single node actions are for)
		bool hovered = ImGui::IsItemHovered();
		if (ImGui::IsWindowHovered() && hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			tab->SetNodeSelected(node, true);
			_lastSelectedNode = node; // already in view
			ImGui::OpenPopup("UIPrefabNodeContextMenu");
		}

		if (ImGui::BeginPopup("UIPrefabNodeContextMenu"))
		{
			Vector<ui2::Node*> targets = tab->IsSelected(node) ? tab->GetSelectedSubtrees() : Vector<ui2::Node*>{node};
			bool               single = (targets.Size() <= 1);
			bool               canRemove = targets.Empty() == false;
			bool               hasPrefabRoot = false; // Not removed: what Cut, Duplicate and Delete tell when greyed out for it
			for (ui2::Node* target : targets)
			{
				canRemove &= tab->CanRemove(target);
				hasPrefabRoot |= UIPrefabEditorTab::IsPrefabRoot(target);
			}

			if (prefabInstance != nullptr && single)
			{
				std::shared_ptr<Asset> prefabAsset = AssetDatabase::GetInstance()->Find(prefabInstance->GetPrefab());
				if (ImGui::MenuItem(ICON_MDI_OPEN_IN_NEW " Open Prefab", nullptr, false, prefabAsset != nullptr))
				{
					Editor::GetInstance()->OpenAsset(prefabAsset);
				}
				if (role == UIPrefabEditorTab::NodeRole::InstanceRoot)
				{
					if (ImGui::MenuItem(ICON_MDI_PACKAGE_VARIANT " Unpack Prefab"))
					{
						tab->UnpackPrefabInstance(node); // Ids only: nothing drawn is changed
					}
					ImGui::SetItemTooltip("Turn the instance into plain nodes of this prefab, no longer following its prefab");
				}
				ImGui::Separator();
			}
			if (ImGui::BeginMenu(ICON_MDI_PLUS_BOX " Add Child", canEditStructure && single))
			{
				for (const auto& descriptorPair : ui2::NodeFactory::GetInstance()->GetAllDescriptors())
				{
					String typeLabel = String(DrawHelper::GetTypeIcon(*descriptorPair.second)) + " " + descriptorPair.second->GetDisplayName();
					if (ImGui::MenuItem(typeLabel.CStr()))
					{
						ui2::Node* newNode = ui2::NodeFactory::GetInstance()->CreateNode(*descriptorPair.second);
						node->AddChild(newNode);

						// Keep the new child visible: a previously leaf/collapsed node would otherwise hide it.
						treeNodeStorage->SetInt(treeNodeId, 1);

						tab->SetSelectedNode(newNode);
						tab->MarkAsDirty(fmt::format("Add {}", descriptorPair.second->GetDisplayName()));
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem(ICON_MDI_RENAME " Rename", "F2", false, UIPrefabEditorTab::CanRename(node) && single))
			{
				StartRename(node);
			}
			if (UIPrefabEditorTab::IsPrefabRoot(node))
			{
				ImGui::SetItemTooltip("Named after the asset: rename the asset to rename it");
			}
			if (ImGui::BeginMenu(ICON_MDI_SWAP_HORIZONTAL " Convert To", canEditStructure && single))
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
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MDI_CONTENT_COPY " Copy", "Ctrl+C"))
			{
				tab->CopyNodes(targets);
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_CUT " Cut", "Ctrl+X", false, canRemove))
			{
				tab->CutNodes(targets); // only marked, moved by the paste
			}
			if (hasPrefabRoot)
			{
				ImGui::SetItemTooltip("The prefab root can't be cut");
			}
			bool canPaste = tab->CanPasteNode();
			if (ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste", "Ctrl+V", false, canPaste))
			{
				_pendingPaste = true; // next to the node, made the active one by the right click
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste As Child", "Ctrl+Shift+V", false, canPaste && canEditStructure && single))
			{
				_pendingPaste = true;
				_pendingPasteParent = node;
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_DUPLICATE " Duplicate", "Ctrl+D", false, canRemove))
			{
				_pendingDuplicateNodes = targets;
			}
			if (hasPrefabRoot)
			{
				ImGui::SetItemTooltip("The prefab root can't be duplicated");
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MDI_DELETE " Delete", "Del", false, canRemove))
			{
				_pendingDeleteNodes = targets;
			}
			if (hasPrefabRoot)
			{
				ImGui::SetItemTooltip("The prefab root can't be deleted");
			}
			ImGui::EndPopup();
		}

		// Last of the row's widgets: everything above queries the tree node as the last item.
		if (renaming)
		{
			// Where the label would start: after the arrow, the icon and the space of "icon name".
			ImGui::SameLine(treeNodeStartX + ImGui::GetTreeNodeToLabelSpacing() + ImGui::CalcTextSize(icon).x + ImGui::CalcTextSize(" ").x - ImGui::GetStyle().FramePadding.x);
			ImGui::SetNextItemWidth(-FLT_MIN);
			// Only once: focusing it every frame would take the focus back as soon as the user clicks elsewhere
			if (_renameFocus)
			{
				ImGui::SetKeyboardFocusHere();
				_renameFocus = false;
			}
			ImGui::InputText("##Rename", _renameBuffer, sizeof(_renameBuffer), ImGuiInputTextFlags_AutoSelectAll);
			_renameSubmitted = true;
			// Enter or focus lost validates, Escape cancels
			if (ImGui::IsItemDeactivated())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Escape) == false)
				{
					ApplyRename();
				}
				_renamedNode = nullptr;
			}
		}

		if (opened)
		{
			for (ui2::Node* child : node->GetChildren())
			{
				DrawNode(child);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}
