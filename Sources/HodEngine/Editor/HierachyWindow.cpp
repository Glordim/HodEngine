#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Scene.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(HierachyWindow, "Hierachy", true)

	struct ChangeParentPayload
	{
		std::vector<std::weak_ptr<game::Entity>>	_entities;
	};

	/// @brief 
	HierachyWindow::HierachyWindow()
	{
	}

	/// @brief 
	void HierachyWindow::DrawContent()
	{
		game::World* world = game::World::GetInstance();
		for (game::Scene* scene : world->GetScenes())
		{
			ImGui::PushID(scene);
			bool open = ImGui::CollapsingHeader(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);
			ImGui::PopID();
			if (open)
			{
				for (auto& pair : scene->GetEntities())
				{
					std::shared_ptr<game::Entity> entity = pair.second;
					if (entity->GetParent().Lock() == nullptr)
					{
						DrawEntity(entity);
					}
				}
			}
		}

		if (ImGui::IsWindowHovered() && ImGui::IsAnyItemHovered() == false && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
		{
			Editor::GetInstance()->SetEntitySelection(nullptr);
		}
		/*
		for (EntityNode* entityNode : _rootEntityNode._children)
		{
			DrawEntityNode(entityNode);
		}
		*/

		if (_openContextualMenu == true || (ImGui::IsWindowHovered() == true && ImGui::IsAnyItemHovered() == false && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true))
		{
			_openContextualMenu = false;
			ImGui::OpenPopup("ContextualMenu");
		}

		if (ImGui::BeginPopup("ContextualMenu") == true)
		{
			std::shared_ptr<game::Entity> selectionLock = Editor::GetInstance()->GetEntitySelection();

			if (ImGui::MenuItem("Create Entity") == true && world->GetScenes().size() > 0)
			{
				std::weak_ptr<game::Entity> entity = world->GetScenes()[0]->CreateEntity("EditMe");
				std::shared_ptr<game::Entity> entityLock = entity.lock();

				if (selectionLock != nullptr)
				{
					entityLock->SetParent(selectionLock);
				}

				Editor::GetInstance()->SetEntitySelection(entityLock);

				Editor::GetInstance()->MarkCurrentSceneAsDirty();

				ImGui::CloseCurrentPopup();
			}

			if (selectionLock != nullptr)
			{
				if (ImGui::MenuItem("Delete") == true)
				{
					world->DestroyEntity(selectionLock);
					Editor::GetInstance()->MarkCurrentSceneAsDirty();
				}

				if (selectionLock->GetPrefabResource() != nullptr && ImGui::MenuItem("Unpack Prefab"))
				{
					selectionLock->SetPrefabResource(nullptr);
					Editor::GetInstance()->MarkCurrentSceneAsDirty();
				}
			}
			ImGui::EndPopup();
		}
	}

	/// @brief 
	/// @param entity 
	void HierachyWindow::DrawEntity(std::weak_ptr<game::Entity> entity)
	{
		std::shared_ptr<game::Entity> entityLock = entity.lock();

		ImGui::PushID(entityLock.get() - 1);
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		//ImGui::Separator();
		//ImGui::Button("", ImVec2(200, 25));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f });
		ImGui::InvisibleButton("", ImVec2(200, 3));
		ImGui::PopStyleVar();
		//ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->IsDataType("EntityId") == true)
			{
				payload = ImGui::AcceptDragDropPayload("EntityId", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
				//GetColorU32(ImGuiCol_DragDropTarget)
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();
				float lerp = (min.y + max.y) * 0.5f;
				min.y = lerp;
				max.y = lerp;
				ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 1.0f); // todo alpha ?
				if ((payload) != nullptr)
				{
					// todo factorize
					uint64_t entityId = *reinterpret_cast<uint64_t*>(payload->Data);
					game::World* world = game::World::GetInstance();
					std::weak_ptr<game::Entity> dropEntity = world->FindEntity(entityId);
					std::shared_ptr<game::Entity> dropEntityLock = dropEntity.lock();
					if (dropEntityLock != nullptr)
					{
						dropEntityLock->SetParent(entityLock->GetParent());
						dropEntityLock->SetSiblingIndex(entityLock->GetSiblingIndex());
						Editor::GetInstance()->MarkCurrentSceneAsDirty();
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleVar();

		std::shared_ptr<game::Entity> selectionLock = Editor::GetInstance()->GetEntitySelection();

		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow;
		if (entityLock->GetChildCount() == 0)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (entityLock == selectionLock)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(entityLock.get());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		bool opened = ImGui::TreeNodeEx("", treeNodeFlags);
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			Editor::GetInstance()->SetEntitySelection(entityLock);
		}
		bool hovered = ImGui::IsItemHovered();
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (ImGui::IsWindowHovered() == true && hovered == true && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true)
		{
			Editor::GetInstance()->SetEntitySelection(entityLock);
			_openContextualMenu = true;
		}

		if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->IsDataType("EntityId") == true)
			{
				//ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

				payload = ImGui::AcceptDragDropPayload("EntityId");
				if ((payload) != nullptr)
				{
					uint64_t entityId = *reinterpret_cast<uint64_t*>(payload->Data);
					game::World* world = game::World::GetInstance();
					std::weak_ptr<game::Entity> dropEntity = world->FindEntity(entityId);
					std::shared_ptr<game::Entity> dropEntityLock = dropEntity.lock();
					if (dropEntityLock != nullptr)
					{
						dropEntityLock->SetParent(entityLock);
						Editor::GetInstance()->MarkCurrentSceneAsDirty();
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginDragDropSource() == true)
		{
			// Some processing...
			ImGui::TextUnformatted(entityLock->GetName().c_str());
			uint64_t entityId = entityLock->GetInstanceId();
			ImGui::SetDragDropPayload("EntityId", (void*)&entityId, sizeof(entityId), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		ImGui::BeginDisabled(entityLock->IsActiveInHierarchy() == false);

		ImGui::SameLine(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f });
		if (entityLock->GetPrefabResource() != nullptr)
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
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(entityLock->GetName().c_str());
		ImGui::PopStyleVar();

		ImGui::EndDisabled();

		if (opened == true)
		{
			uint32_t childCount = entityLock->GetChildCount();
			for (uint32_t childIndex = 0; childIndex < childCount; ++childIndex)
			{
				std::shared_ptr<game::Entity> childEntity = entityLock->GetChild(childIndex).Lock();
				if (childEntity != nullptr)
				{
					DrawEntity(childEntity);
				}
			}
			ImGui::TreePop();
		}
	}
/*
	/// @brief 
	/// @param entityNode 
	void HierachyWindow::DrawEntityNode(EntityNode* entityNode)
	{
		std::shared_ptr<game::Entity> selectionLock = _selection.lock();

		std::shared_ptr<game::Entity> entityLock = entityNode->_entity.lock();

		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_FramePadding;
		if (entityNode->_children.empty() == true)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (entityLock == selectionLock)
		{
			treeNodeFlags|= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(entityLock.get());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		bool opened = ImGui::TreeNodeEx(entityLock->GetName().c_str(), treeNodeFlags);
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			_selection = entityLock;
			Editor::GetInstance()->SetEntitySelection(_selection);
		}
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (ImGui::IsWindowHovered() == true && ImGui::IsItemHovered() == false && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true)
		{
			_selection = entityLock;
			Editor::GetInstance()->SetEntitySelection(_selection);

			ImGui::OpenPopup("ContextualMenu");
		}

		if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->IsDataType("EntityId") == true)
			{
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

				payload = ImGui::AcceptDragDropPayload("EntityId");
				if ((payload) != nullptr)
				{
					game::Entity::Id entityId = *reinterpret_cast<game::Entity::Id*>(payload->Data);
					game::World* world = game::World::GetInstance();
					std::weak_ptr<game::Entity> dropEntity = world->FindEntity(entityId);
					std::shared_ptr<game::Entity> dropEntityLock = dropEntity.lock();
					if (dropEntityLock != nullptr)
					{
						// todo get or add component
						std::shared_ptr<game::NodeComponent> parentNodeComponentLock = entityLock->GetComponent<game::NodeComponent>().lock();
						if (parentNodeComponentLock == nullptr)
						{
							parentNodeComponentLock = entityLock->AddComponent<game::NodeComponent>().lock();
						}

						// todo get or add component
						std::shared_ptr<game::NodeComponent> dropNodeComponentLock = dropEntityLock->GetComponent<game::NodeComponent>().lock();
						if (dropNodeComponentLock == nullptr)
						{
							dropNodeComponentLock = dropEntityLock->AddComponent<game::NodeComponent>().lock();
						}
						dropNodeComponentLock->SetParent(parentNodeComponentLock);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginDragDropSource() == true)
		{
			// Some processing...
			ImGui::TextUnformatted(entityLock->GetName().c_str());
			game::Entity::Id entityId = entityLock->GetId();
			ImGui::SetDragDropPayload("EntityId", (void*)&entityId, sizeof(entityId), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		if (opened == true)
		{
			for (EntityNode* entityNode : entityNode->_children)
			{
				DrawEntityNode(entityNode);
			}
			ImGui::TreePop();
		}
	}
	*/
}
