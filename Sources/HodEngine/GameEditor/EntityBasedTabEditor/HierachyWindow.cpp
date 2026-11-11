#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/HierachyWindow.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"

#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"

#include <HodEngine/Core/Math.hpp>

#include <algorithm>
#include <cstring>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(HierachyWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	HierachyWindow::HierachyWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Hierarchy");
	}

	/// @brief
	void HierachyWindow::DrawContent()
	{
		EntityEditorTab* tab = GetOwner<EntityEditorTab>();
		game::World*     world = tab->GetWorld();

		// F centers the viewport on the selection, as in the viewport itself
		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_F, false) && ImGui::GetIO().WantTextInput == false)
		{
			tab->RequestFrameSelection();
		}

		// A newly active entity may sit under collapsed parents (picked in the viewport, pasted...): for that one frame its ancestors are
		// forced open and it is scrolled into view. Only on a change, the user stays free to collapse them again. Not for the selections
		// made here (see the end of the multi-selection scope), of entities already in view.
		Entity* activeEntity = tab->GetEntitySelection();
		_revealSelection = (activeEntity != nullptr && activeEntity != _lastActiveEntity.Get());
		_lastActiveEntity = activeEntity;

		// Escape is for a pending cut first
		ImGuiMultiSelectFlags multiSelectFlags = ImGuiMultiSelectFlags_None;
		if (tab->HasCutEntities() == false)
		{
			multiSelectFlags |= ImGuiMultiSelectFlags_ClearOnEscape;
		}
		// ImGui sees a click in a context menu as a click on the void of the scope (the popup is in this window hierarchy): it would
		// clear the selection the menu acts on (see AssetBrowserWindow). No box-selection: Ctrl/Shift+click are enough in a tree.
		if (ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId) == false)
		{
			multiSelectFlags |= ImGuiMultiSelectFlags_ClearOnClickVoid;
		}
		// The multi-selection always draws ImGui's nav cursor (the keyboard focus outline), which any click moves to the clicked entity, its
		// arrow included: it would look like the selection of an entity only opened. Only shown while navigating with the keyboard.
		bool hideNavCursor = (ImGui::GetCurrentContext()->NavCursorVisible == false);
		if (hideNavCursor)
		{
			ImGui::PushStyleColor(ImGuiCol_NavCursor, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		}
		ImGuiMultiSelectIO* multiSelectIO = ImGui::BeginMultiSelect(multiSelectFlags, static_cast<int>(tab->GetSelectedEntities().Size()), -1);
		ApplySelectionRequests(multiSelectIO, false);
		_visibleEntities.Clear();

		// A header per scene, but for a prefab: its root entity, named after it as the scene is, is the whole content (the playing world's
		// copy of it while playing)
		bool sceneHeaders = (tab->GetRootEntity() == nullptr);
		for (Scene* scene : world->GetScenes())
		{
			bool open = true;
			if (sceneHeaders)
			{
				ImGui::PushID(scene);
				open = ImGui::CollapsingHeader(scene->GetName().CStr(), ImGuiTreeNodeFlags_DefaultOpen);
				ImGui::PopID();
			}
			if (open)
			{
				// In a stable order (an unordered map's would change with the entities rebuilt by an undo)
				for (Entity* entity : scene->GetRootEntities())
				{
					DrawEntity(entity);
				}
			}
		}

		multiSelectIO = ImGui::EndMultiSelect();
		if (hideNavCursor)
		{
			ImGui::PopStyleColor();
		}
		ApplySelectionRequests(multiSelectIO, true);
		_lastActiveEntity = tab->GetEntitySelection(); // made here: no reveal

		// The rename field was not drawn this frame (its entity got collapsed, deleted...): validated now, it can't lose the focus anymore.
		// Unless the rename has just started and its field is yet to be drawn.
		if (_renamedEntity.Get() != nullptr && _renameDrawn == false && _renameFocus == false)
		{
			ApplyRename();
			_renamedEntity = nullptr;
		}
		_renameDrawn = false;

		// The right click on the void: the menu to create an entity (under the root of a prefab)
		if (_openContextualMenu == true || (ImGui::IsWindowHovered() == true && ImGui::IsAnyItemHovered() == false && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true))
		{
			if (_openContextualMenu == false)
			{
				tab->SetEntitySelection(nullptr);
			}
			_openContextualMenu = false;
			ImGui::OpenPopup("ContextualMenu");
		}
		DrawContextMenu();

		HandleShortcuts();

		ApplyPendingEdits();

		if (_selectAllRequested)
		{
			tab->SetSelectedEntities(_visibleEntities);
			_lastActiveEntity = tab->GetEntitySelection(); // made here: no reveal
			_selectAllRequested = false;
		}
	}

	/// @brief The edits asked while the tree was drawn, done once it is (they change it)
	void HierachyWindow::ApplyPendingEdits()
	{
		EntityEditorTab* tab = GetOwner<EntityEditorTab>();

		if (_pendingReparent)
		{
			Vector<Entity*> entities;
			for (const WeakPtr<Entity>& entity : _pendingReparentEntities)
			{
				if (entity.Get() != nullptr)
				{
					entities.PushBack(entity.Get());
				}
			}
			Entity* parent = _pendingReparentParent.Get();
			if (entities.Empty() == false && (_pendingReparentToRoot || parent != nullptr))
			{
				tab->ReparentEntities(entities, parent, _pendingReparentIndex);
			}
		}
		_pendingReparent = false;
		_pendingReparentToRoot = false;
		_pendingReparentEntities.Clear();
		_pendingReparentParent = nullptr;
		_pendingReparentIndex = UINT32_MAX;

		if (_pendingPaste)
		{
			if (_pendingPasteInSelection)
			{
				tab->PasteInSelection();
			}
			else
			{
				tab->PasteNextToSelection();
			}
		}
		_pendingPaste = false;
		_pendingPasteInSelection = false;

		if (_pendingDuplicate)
		{
			tab->DuplicateEntities(tab->GetSelectedSubtrees());
		}
		_pendingDuplicate = false;

		if (_pendingDelete)
		{
			Vector<Entity*> deleted = tab->GetSelectedSubtrees();
			for (Entity* entity : deleted)
			{
				for (Entity* renamed = _renamedEntity.Get(); renamed != nullptr; renamed = renamed->GetParent().Lock())
				{
					if (renamed == entity)
					{
						_renamedEntity = nullptr;
						break;
					}
				}
			}
			tab->DeleteEntities(deleted);
		}
		_pendingDelete = false;
	}

	/// @brief
	/// @param multiSelectIO
	/// @param scopeEnd
	void HierachyWindow::ApplySelectionRequests(ImGuiMultiSelectIO* multiSelectIO, bool scopeEnd)
	{
		// The nav init of a newly focused window puts the nav on the first item, and the multi-selection auto-selects it as if it was reached
		// with the arrows: only a real move selects (see AssetBrowserWindow)
		const ImGuiContext& g = *ImGui::GetCurrentContext();
		if (g.NavJustMovedToId != 0 && g.NavJustMovedToIsInit)
		{
			return;
		}

		EntityEditorTab* tab = GetOwner<EntityEditorTab>();
		for (const ImGuiSelectionRequest& request : multiSelectIO->Requests)
		{
			if (request.Type == ImGuiSelectionRequestType_SetAll)
			{
				if (request.Selected == false)
				{
					tab->SetEntitySelection(nullptr);
				}
				else if (scopeEnd)
				{
					tab->SetSelectedEntities(_visibleEntities);
				}
				else
				{
					_selectAllRequested = true; // _visibleEntities is last frame's, its entities may be gone
				}
			}
			else if (request.Type == ImGuiSelectionRequestType_SetRange && scopeEnd)
			{
				// From the range's first item to its last one, so that the last one (the clicked one) ends up the active one. An end no longer
				// drawn (collapsed since) leaves the other one only.
				Entity* firstEntity = reinterpret_cast<Entity*>(static_cast<intptr_t>(request.RangeFirstItem));
				Entity* lastEntity = reinterpret_cast<Entity*>(static_cast<intptr_t>(request.RangeLastItem));
				auto    firstIt = std::find(_visibleEntities.Begin(), _visibleEntities.End(), firstEntity);
				auto    lastIt = std::find(_visibleEntities.Begin(), _visibleEntities.End(), lastEntity);
				if (firstIt == _visibleEntities.End())
				{
					firstIt = lastIt;
				}
				else if (lastIt == _visibleEntities.End())
				{
					lastIt = firstIt;
				}
				if (firstIt == _visibleEntities.End())
				{
					continue;
				}

				int32_t first = static_cast<int32_t>(firstIt - _visibleEntities.Begin());
				int32_t last = static_cast<int32_t>(lastIt - _visibleEntities.Begin());
				int32_t step = (first <= last) ? 1 : -1;
				for (int32_t index = first;; index += step)
				{
					tab->SetEntitySelected(_visibleEntities[static_cast<uint32_t>(index)], request.Selected);
					if (index == last)
					{
						break;
					}
				}
			}
		}
	}

	/// @brief F2 renames the active entity, Ctrl+D duplicates, Delete deletes the selected ones, and the clipboard's (see
	/// EntityEditorTab::HandleClipboardShortcuts). Not while a text is typed (the rename field...). After the tree is drawn.
	void HierachyWindow::HandleShortcuts()
	{
		EntityEditorTab* tab = GetOwner<EntityEditorTab>();
		if (ImGui::IsWindowFocused() == false || ImGui::GetIO().WantTextInput || _renamedEntity.Get() != nullptr)
		{
			return;
		}

		tab->HandleClipboardShortcuts();

		Entity* selection = tab->GetEntitySelection();
		if (selection == nullptr)
		{
			return;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_F2, false))
		{
			StartRename(selection);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
		{
			_pendingDelete = true;
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_D))
		{
			_pendingDuplicate = true;
		}
	}

	/// @brief On the selection (the right clicked entity made the active one): what can be done to it. On the void: creating an entity.
	void HierachyWindow::DrawContextMenu()
	{
		if (ImGui::BeginPopup("ContextualMenu") == false)
		{
			return;
		}

		EntityEditorTab* tab = GetOwner<EntityEditorTab>();
		game::World*     world = tab->GetWorld();
		Entity*          selection = tab->GetEntitySelection();
		Vector<Entity*>  targets = tab->GetSelectedSubtrees();
		bool             single = (tab->GetSelectedEntities().Size() <= 1);
		bool             hasRoot = false; // Not removed: what Cut, Duplicate and Delete tell when greyed out for it
		for (Entity* target : targets)
		{
			hasRoot |= tab->IsRootEntity(target);
		}

		const char* createLabel = (selection != nullptr) ? ICON_MDI_PLUS_BOX " Create Child Entity" : ICON_MDI_PLUS_BOX " Create Entity";
		if (ImGui::MenuItem(createLabel, nullptr, false, single) && world->GetScenes().Size() > 0)
		{
			Entity* entity = world->GetScenes()[0]->CreateEntity("Entity");

			// Under the selection, or the root entity (a prefab has one root)
			Entity* parent = selection != nullptr ? selection : tab->GetDefaultParent();
			if (parent != nullptr)
			{
				entity->SetParent(parent);
			}

			tab->SetEntitySelection(entity);
			tab->MarkAsDirty("Create Entity");
			StartRename(entity);
		}
		if (selection == nullptr && ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste", "Ctrl+V", false, tab->CanPaste()))
		{
			_pendingPaste = true; // no selection: under the default parent
		}

		if (selection != nullptr)
		{
			std::shared_ptr<PrefabResource> prefabResource = selection->GetPrefabResource();
			if (prefabResource != nullptr && single)
			{
				ImGui::Separator();
				std::shared_ptr<Asset> prefabAsset = AssetDatabase::GetInstance()->Find(prefabResource->GetUid());
				if (ImGui::MenuItem(ICON_MDI_OPEN_IN_NEW " Open Prefab", nullptr, false, prefabAsset != nullptr))
				{
					Editor::GetInstance()->OpenAsset(prefabAsset);
				}
				if (ImGui::MenuItem(ICON_MDI_PACKAGE_VARIANT " Unpack Prefab"))
				{
					selection->SetPrefabResource(nullptr);
					tab->MarkAsDirty("Unpack Prefab");
				}
				ImGui::SetItemTooltip("Turn the instance into plain entities, no longer following its prefab");
			}

			ImGui::Separator();
			bool isRoot = tab->IsRootEntity(selection);
			if (ImGui::MenuItem(ICON_MDI_RENAME " Rename", "F2", false, isRoot == false && single))
			{
				StartRename(selection);
			}
			if (isRoot)
			{
				ImGui::SetItemTooltip("Named after the asset: rename the asset to rename it");
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MDI_CONTENT_COPY " Copy", "Ctrl+C"))
			{
				tab->CopyEntities(targets);
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_CUT " Cut", "Ctrl+X", false, hasRoot == false))
			{
				tab->CutEntities(targets); // only marked, moved by the paste
			}
			if (hasRoot)
			{
				ImGui::SetItemTooltip("The prefab's root entity can't be cut");
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste", "Ctrl+V", false, tab->CanPaste()))
			{
				_pendingPaste = true;
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste As Child", "Ctrl+Shift+V", false, tab->CanPaste() && single))
			{
				_pendingPaste = true;
				_pendingPasteInSelection = true;
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_DUPLICATE " Duplicate", "Ctrl+D", false, hasRoot == false))
			{
				_pendingDuplicate = true;
			}
			if (hasRoot)
			{
				ImGui::SetItemTooltip("The prefab's root entity can't be duplicated");
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MDI_DELETE " Delete", "Del", false, hasRoot == false))
			{
				_pendingDelete = true;
			}
			if (hasRoot)
			{
				ImGui::SetItemTooltip("The prefab's root entity can't be deleted");
			}
		}
		ImGui::EndPopup();
	}

	/// @brief
	/// @param entity
	void HierachyWindow::StartRename(Entity* entity)
	{
		if (entity == nullptr || GetOwner<EntityEditorTab>()->IsRootEntity(entity))
		{
			return;
		}
		_renamedEntity = entity;
		std::strncpy(_renameBuffer, entity->GetName().CStr(), sizeof(_renameBuffer) - 1);
		_renameBuffer[sizeof(_renameBuffer) - 1] = '\0';
		_renameFocus = true;
	}

	/// @brief
	void HierachyWindow::ApplyRename()
	{
		Entity* entity = _renamedEntity.Get();
		if (entity != nullptr && entity->GetName() != _renameBuffer)
		{
			entity->SetName(_renameBuffer);
			GetOwner()->MarkAsDirty("Rename");
		}
	}

	/// @brief The entities a drag of entity moves: the selected ones when it is one of them, only it otherwise
	/// @param entity
	/// @return
	Vector<Entity*> HierachyWindow::GetDraggedEntities(Entity* entity) const
	{
		EntityEditorTab* tab = const_cast<HierachyWindow*>(this)->GetOwner<EntityEditorTab>();
		if (tab->IsSelected(entity))
		{
			return tab->GetSelectedSubtrees();
		}
		return Vector<Entity*>{entity};
	}

	/// @brief
	/// @param entity
	void HierachyWindow::DrawEntity(Entity* entity)
	{
		EntityEditorTab* tab = GetOwner<EntityEditorTab>();

		// Drop zone to put entities before this one: the gap between the rows (ItemSpacing), not taking any room of its own, so that the
		// rows are as high and as close as in the other trees (UI prefab editor...)
		float rowSpacing = ImGui::GetStyle().ItemSpacing.y;
		ImGui::PushID(entity - 1);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - rowSpacing);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {ImGui::GetStyle().ItemSpacing.x, 0.0f});
		ImGui::InvisibleButton("", ImVec2(Max(ImGui::GetContentRegionAvail().x, 1.0f), Max(rowSpacing, 1.0f)));
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* peekedPayload = ImGui::GetDragDropPayload();
			if (peekedPayload->IsDataType("EntityId") == true)
			{
				// Next to entity: under its parent (not out of the prefab's root, nor into the dragged entities themselves)
				Entity*         draggedEntity = reinterpret_cast<EntityDragAndDropPayload*>(peekedPayload->Data)->_entity;
				Vector<Entity*> draggedEntities = GetDraggedEntities(draggedEntity);
				Entity*         parent = entity->GetParent().Lock();
				bool            canDrop = std::find(draggedEntities.Begin(), draggedEntities.End(), entity) == draggedEntities.End();
				for (Entity* dragged : draggedEntities)
				{
					// Among the roots of entity's scene: only the entities of that scene (a root stays in its scene)
					canDrop &= tab->CanReparent(dragged, parent) && (parent != nullptr || dragged->GetScene() == entity->GetScene());
				}

				const ImGuiPayload* payload = canDrop ? ImGui::AcceptDragDropPayload("EntityId", ImGuiDragDropFlags_AcceptNoDrawDefaultRect) : nullptr;
				if (canDrop)
				{
					ImVec2 min = ImGui::GetItemRectMin();
					ImVec2 max = ImGui::GetItemRectMax();
					float  middle = (min.y + max.y) * 0.5f;
					ImGui::GetWindowDrawList()->AddLine(ImVec2(min.x, middle), ImVec2(max.x, middle), ImGui::GetColorU32(ImGuiCol_DragDropTarget), 1.0f);
				}
				if (payload != nullptr)
				{
					// Its place among the siblings that stay (the roots of its scene when it has no parent)
					Vector<Entity*> siblings;
					if (parent != nullptr)
					{
						for (const WeakEntity& sibling : parent->GetChildren())
						{
							siblings.PushBack(sibling.Lock());
						}
					}
					else if (entity->GetScene() != nullptr)
					{
						siblings = entity->GetScene()->GetRootEntities();
					}
					uint32_t index = 0;
					for (Entity* sibling : siblings)
					{
						if (sibling == entity)
						{
							break;
						}
						if (std::find(draggedEntities.Begin(), draggedEntities.End(), sibling) == draggedEntities.End())
						{
							++index;
						}
					}

					_pendingReparent = true;
					_pendingReparentEntities.Clear();
					for (Entity* dragged : draggedEntities)
					{
						_pendingReparentEntities.PushBack(dragged);
					}
					_pendingReparentParent = parent;
					_pendingReparentToRoot = (parent == nullptr);
					_pendingReparentIndex = index;
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes;
		if (entity->GetChildren().Empty())
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (tab->IsSelected(entity))
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		Entity* activeEntity = tab->GetEntitySelection();
		if (_revealSelection)
		{
			for (Entity* ancestor = activeEntity->GetParent().Lock(); ancestor != nullptr; ancestor = ancestor->GetParent().Lock())
			{
				if (ancestor == entity)
				{
					ImGui::SetNextItemOpen(true, ImGuiCond_Always);
					break;
				}
			}
		}

		// Ids identify the entity, names may collide between siblings: the ImGui id is the entity itself. Clicks are the multi-selection's
		// (see ApplySelectionRequests).
		ImGui::PushID(entity);
		ImGui::SetNextItemSelectionUserData(static_cast<ImGuiSelectionUserData>(reinterpret_cast<intptr_t>(entity)));
		_visibleEntities.PushBack(entity);
		bool opened = ImGui::TreeNodeEx("", treeNodeFlags);
		// Kept to open the tree node when something is dropped into it
		ImGuiID treeNodeId = ImGui::GetItemID();

		if (_revealSelection && entity == activeEntity)
		{
			ImGui::SetScrollHereY();
		}

		bool hovered = ImGui::IsItemHovered();
		ImGui::PopID();

		if (ImGui::BeginItemTooltip())
		{
			std::string_view internalStateLabel = ReflectedEnum<Entity::InternalState>::GetEnumDescriptor().ToString(entity->GetInternalState());
			ImGui::Text("InternalState: %.*s", (int)internalStateLabel.size(), internalStateLabel.data());
			ImGui::EndTooltip();
		}

		// The right click selects the entity if it isn't (the multi-selection's, on mouse down): the menu acts on the selection, the entity
		// made the active one (the one the single entity actions are for)
		if (ImGui::IsWindowHovered() == true && hovered == true && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true)
		{
			tab->SetEntitySelected(entity, true);
			_lastActiveEntity = entity; // already in view
			_openContextualMenu = true;
		}

		// Into entity: only offered when the move makes sense (not into one of the dragged entities, nor out of the prefab's root)
		if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* peekedPayload = ImGui::GetDragDropPayload();
			if (peekedPayload->IsDataType("EntityId") == true)
			{
				Entity*         draggedEntity = reinterpret_cast<EntityDragAndDropPayload*>(peekedPayload->Data)->_entity;
				Vector<Entity*> draggedEntities = GetDraggedEntities(draggedEntity);
				if (tab->CanReparentEntities(draggedEntities, entity) && ImGui::AcceptDragDropPayload("EntityId") != nullptr)
				{
					_pendingReparent = true;
					_pendingReparentEntities.Clear();
					for (Entity* dragged : draggedEntities)
					{
						_pendingReparentEntities.PushBack(dragged);
					}
					_pendingReparentParent = entity;
					_pendingReparentIndex = UINT32_MAX;

					// Keep the moved entities visible: a previously leaf/collapsed target would otherwise hide them
					ImGui::GetStateStorage()->SetInt(treeNodeId, 1);
				}
			}
			ImGui::EndDragDropTarget();
		}

		// The root entity of a prefab stays where it is. The selection goes along when the entity is in it.
		if (tab->IsRootEntity(entity) == false && ImGui::BeginDragDropSource() == true)
		{
			uint32_t draggedCount = GetDraggedEntities(entity).Size();
			if (draggedCount > 1)
			{
				ImGui::Text("%s (+%u)", entity->GetName().CStr(), draggedCount - 1);
			}
			else
			{
				ImGui::TextUnformatted(entity->GetName().CStr());
			}

			_entityDragAndDropPayload._hierarchyWindow = this;
			_entityDragAndDropPayload._entity = entity;
			ImGui::SetDragDropPayload("EntityId", (void*)&_entityDragAndDropPayload, sizeof(_entityDragAndDropPayload), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		// Greyed: inactive, or cut (until pasted, as in a file explorer)
		ImGui::BeginDisabled(entity->IsActiveInHierarchy() == false || tab->IsCut(entity));

		ImGui::SameLine(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {ImGui::GetStyle().ItemSpacing.x, 0.0f});
		if (entity->GetPrefabResource() != nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(ICON_MDI_CUBE);
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(ICON_MDI_CUBE_OUTLINE);
		}
		ImGui::SameLine();
		bool renaming = (entity == _renamedEntity.Get());
		if (renaming == false)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(entity->GetName().CStr());
		}
		ImGui::PopStyleVar();

		ImGui::EndDisabled();

		if (renaming)
		{
			// Not disabled with an inactive entity's label: editable
			ImGui::SetNextItemWidth(-FLT_MIN);
			// Only once: focusing it every frame would take the focus back as soon as the user clicks elsewhere
			if (_renameFocus)
			{
				ImGui::SetKeyboardFocusHere();
				_renameFocus = false;
			}
			ImGui::PushID(entity);
			ImGui::InputText("##Rename", _renameBuffer, sizeof(_renameBuffer), ImGuiInputTextFlags_AutoSelectAll);
			ImGui::PopID();
			_renameDrawn = true;
			// Enter or focus lost validates, Escape cancels
			if (ImGui::IsItemDeactivated())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Escape) == false)
				{
					ApplyRename();
				}
				_renamedEntity = nullptr;
			}
		}

		if (opened == true)
		{
			for (const WeakEntity& child : entity->GetChildren())
			{
				Entity* childEntity = child.Lock();
				if (childEntity != nullptr)
				{
					DrawEntity(childEntity);
				}
			}
			ImGui::TreePop();
		}
	}
}
