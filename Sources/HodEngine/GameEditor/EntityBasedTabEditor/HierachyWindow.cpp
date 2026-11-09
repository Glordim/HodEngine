#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/HierachyWindow.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"

#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"

#include <HodEngine/Core/Math.hpp>

#include <cstring>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(HierachyWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	struct ChangeParentPayload
	{
		Vector<std::weak_ptr<Entity>> _entities;
	};

	/// @brief
	HierachyWindow::HierachyWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Hierarchy");
	}

	/// @brief
	void HierachyWindow::DrawContent()
	{
		game::World* world = GetOwner<EntityEditorTab>()->GetWorld();

		// F centers the viewport on the selection, as in the viewport itself
		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_F, false) && ImGui::GetIO().WantTextInput == false)
		{
			GetOwner<EntityEditorTab>()->RequestFrameSelection();
		}

		// A header per scene, but for a prefab: its root entity, named after it as the scene is, is the whole content (the playing world's
		// copy of it while playing)
		bool sceneHeaders = (GetOwner<EntityEditorTab>()->GetRootEntity() == nullptr);
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
				for (auto& pair : scene->GetEntities())
				{
					Entity* entity = pair.second;
					if (entity->GetParent().Lock() == nullptr)
					{
						DrawEntity(entity);
					}
				}
			}
		}
		_previousSelection = GetOwner<EntityEditorTab>()->GetEntitySelection();

		// The rename field was not drawn this frame (its entity got collapsed, deleted...): validated now, it can't lose the focus anymore.
		// Unless the rename has just started and its field is yet to be drawn.
		if (_renamedEntity.Get() != nullptr && _renameDrawn == false && _renameFocus == false)
		{
			ApplyRename();
			_renamedEntity = nullptr;
		}
		_renameDrawn = false;

		if (ImGui::IsWindowHovered() && ImGui::IsAnyItemHovered() == false && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
		{
			GetOwner<EntityEditorTab>()->SetEntitySelection(nullptr);
		}

		if (_openContextualMenu == true || (ImGui::IsWindowHovered() == true && ImGui::IsAnyItemHovered() == false && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true))
		{
			_openContextualMenu = false;
			ImGui::OpenPopup("ContextualMenu");
		}
		DrawContextMenu();

		HandleShortcuts();

		if (_pendingPaste)
		{
			if (_pendingPasteInSelection)
			{
				GetOwner<EntityEditorTab>()->PasteInSelection();
			}
			else
			{
				GetOwner<EntityEditorTab>()->PasteNextToSelection();
			}
		}
		_pendingPaste = false;
		_pendingPasteInSelection = false;

		if (Entity* entity = _pendingDuplicate.Get())
		{
			GetOwner<EntityEditorTab>()->DuplicateEntity(entity);
		}
		_pendingDuplicate = nullptr;

		if (Entity* entity = _pendingDelete.Get())
		{
			for (Entity* renamed = _renamedEntity.Get(); renamed != nullptr; renamed = renamed->GetParent().Lock())
			{
				if (renamed == entity)
				{
					_renamedEntity = nullptr;
					break;
				}
			}
			GetOwner<EntityEditorTab>()->DeleteEntity(entity);
		}
		_pendingDelete = nullptr;
	}

	/// @brief F2 renames, Ctrl+D duplicates, Delete deletes the selected entity, and the clipboard's (see
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
		if (selection == nullptr || tab->IsRootEntity(selection))
		{
			return;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_F2, false))
		{
			StartRename(selection);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
		{
			_pendingDelete = selection;
		}
		else if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D, false))
		{
			_pendingDuplicate = selection;
		}
	}

	/// @brief On an entity (made the selection by the right click): what can be done to it. On the void: creating one.
	void HierachyWindow::DrawContextMenu()
	{
		if (ImGui::BeginPopup("ContextualMenu") == false)
		{
			return;
		}

		EntityEditorTab* tab = GetOwner<EntityEditorTab>();
		game::World*     world = tab->GetWorld();
		Entity*          selection = tab->GetEntitySelection();

		const char* createLabel = (selection != nullptr) ? ICON_MDI_PLUS_BOX " Create Child Entity" : ICON_MDI_PLUS_BOX " Create Entity";
		if (ImGui::MenuItem(createLabel) && world->GetScenes().Size() > 0)
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
			bool                            isRoot = tab->IsRootEntity(selection);
			std::shared_ptr<PrefabResource> prefabResource = selection->GetPrefabResource();
			if (prefabResource != nullptr)
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
			if (ImGui::MenuItem(ICON_MDI_RENAME " Rename", "F2", false, isRoot == false))
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
				tab->CopyEntities({selection});
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_CUT " Cut", "Ctrl+X", false, isRoot == false))
			{
				tab->CutEntities({selection}); // only marked, moved by the paste
			}
			if (isRoot)
			{
				ImGui::SetItemTooltip("The prefab's root entity can't be cut");
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste", "Ctrl+V", false, tab->CanPaste()))
			{
				_pendingPaste = true;
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste As Child", "Ctrl+Shift+V", false, tab->CanPaste()))
			{
				_pendingPaste = true;
				_pendingPasteInSelection = true;
			}
			if (ImGui::MenuItem(ICON_MDI_CONTENT_DUPLICATE " Duplicate", "Ctrl+D", false, isRoot == false))
			{
				_pendingDuplicate = selection;
			}
			if (isRoot)
			{
				ImGui::SetItemTooltip("The prefab's root entity can't be duplicated");
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MDI_DELETE " Delete", "Del", false, isRoot == false))
			{
				_pendingDelete = selection;
			}
			if (isRoot)
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

	/// @brief
	/// @param entity
	void HierachyWindow::DrawEntity(Entity* entity)
	{
		// Drop zone to put an entity before this one: the gap between the rows (ItemSpacing), not taking any room of its own,
		// so that the rows are as high and as close as in the other trees (UI prefab editor...)
		float rowSpacing = ImGui::GetStyle().ItemSpacing.y;
		ImGui::PushID(entity - 1);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - rowSpacing);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {ImGui::GetStyle().ItemSpacing.x, 0.0f});
		ImGui::InvisibleButton("", ImVec2(Max(ImGui::GetContentRegionAvail().x, 1.0f), Max(rowSpacing, 1.0f)));
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->IsDataType("EntityId") == true)
			{
				payload = ImGui::AcceptDragDropPayload("EntityId", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
				// GetColorU32(ImGuiCol_DragDropTarget)
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();
				float  lerp = (min.y + max.y) * 0.5f;
				min.y = lerp;
				max.y = lerp;
				ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 1.0f); // todo alpha ?
				if ((payload) != nullptr)
				{
					// todo factorize
					EntityDragAndDropPayload* entityDragAndDropPayload = reinterpret_cast<EntityDragAndDropPayload*>(payload->Data);
					Entity*             dropEntity = entityDragAndDropPayload->_entity;
					// Next to entity: under its parent (not out of the prefab's root, nor into the dragged entity itself)
					if (dropEntity != nullptr && dropEntity != entity && GetOwner<EntityEditorTab>()->CanReparent(dropEntity, entity->GetParent().Lock()))
					{
						dropEntity->SetParent(entity->GetParent());
						dropEntity->SetSiblingIndex(entity->GetSiblingIndex());
						GetOwner()->MarkAsDirty("Move");
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		Entity* selection = GetOwner<EntityEditorTab>()->GetEntitySelection();

		ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes;
		if (entity->GetChildren().Empty())
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (entity == selection)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		if (_previousSelection != selection)
		{
			Entity* selectionIt = selection;
			while (selectionIt != nullptr)
			{
				if (selectionIt == entity)
				{
					ImGui::SetNextItemOpen(true, ImGuiCond_Always);
					break;
				}
				selectionIt = selectionIt->GetParent().Lock();
			}
		}

		ImGui::PushID(entity);
		bool opened = ImGui::TreeNodeEx("", treeNodeFlags);
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			GetOwner<EntityEditorTab>()->SetEntitySelection(entity);
		}

		if (entity == selection && _previousSelection != selection)
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

		if (ImGui::IsWindowHovered() == true && hovered == true && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true)
		{
			GetOwner<EntityEditorTab>()->SetEntitySelection(entity);
			_openContextualMenu = true;
		}

		if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->IsDataType("EntityId") == true)
			{
				// ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

				payload = ImGui::AcceptDragDropPayload("EntityId");
				if ((payload) != nullptr)
				{
					EntityDragAndDropPayload* entityDragAndDropPayload = reinterpret_cast<EntityDragAndDropPayload*>(payload->Data);
					Entity*             dropEntity = entityDragAndDropPayload->_entity;
					if (dropEntity != nullptr && GetOwner<EntityEditorTab>()->CanReparent(dropEntity, entity))
					{
						dropEntity->SetParent(entity);
						GetOwner()->MarkAsDirty("Move");
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// The root entity of a prefab stays where it is
		if (GetOwner<EntityEditorTab>()->IsRootEntity(entity) == false && ImGui::BeginDragDropSource() == true)
		{
			// Some processing...
			ImGui::TextUnformatted(entity->GetName().CStr());

			_entityDragAndDropPayload._hierarchyWindow = this;
			_entityDragAndDropPayload._entity = entity;
			ImGui::SetDragDropPayload("EntityId", (void*)&_entityDragAndDropPayload, sizeof(_entityDragAndDropPayload), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		// Greyed: inactive, or cut (until pasted, as in a file explorer)
		ImGui::BeginDisabled(entity->IsActiveInHierarchy() == false || GetOwner<EntityEditorTab>()->IsCut(entity));

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
