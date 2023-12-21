#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Scene.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(HierachyWindow, "Hierachy", true)

	struct ChangeParentPayload
	{
		std::vector<std::weak_ptr<game::Entity>>	_entities;
	};

	/// @brief 
	HierachyWindow::HierachyWindow()
	: _onAddEntityCallback(std::bind(&HierachyWindow::OnAddEntityCallback, this, std::placeholders::_1))
	, _onRemoveEntityCallback(std::bind(&HierachyWindow::OnRemoveEntityCallback, this, std::placeholders::_1))
	, _onRenameEntityCallback(std::bind(&HierachyWindow::OnRenameEntityCallback, this, std::placeholders::_1))
	, _onAddComponentCallback(std::bind(&HierachyWindow::OnAddComponentCallback, this, std::placeholders::_1))
	, _onRemoveComponentCallback(std::bind(&HierachyWindow::OnRemoveComponentCallback, this, std::placeholders::_1))
	{
		game::World* world = game::World::GetInstance();

		world->GetAddEntityEvent() += _onAddEntityCallback;
		world->GetRemoveEntityEvent() += _onRemoveEntityCallback;
		world->GetRenameEntityEvent() += _onRenameEntityCallback;
	}

	/// @brief 
	/// @param entity 
	void HierachyWindow::OnAddEntityCallback(std::weak_ptr<game::Entity> entity)
	{
		/*
		std::shared_ptr<game::Entity> entityLock = entity.lock();
		if (entityLock != nullptr)
		{
			EntityNode* entityNode = new EntityNode();
			entityNode->_entity = entity;

			entityLock->GetAddComponentEvent() += _onAddComponentCallback;
			entityLock->GetRemoveComponentEvent() += _onRemoveComponentCallback;

			std::shared_ptr<game::NodeComponent> parentLock;
			{
				std::weak_ptr<game::NodeComponent> nodeComponent = entityLock->GetComponent<game::NodeComponent>();
				std::shared_ptr<game::NodeComponent> nodeComponentLock = nodeComponent.lock();
				if (nodeComponentLock != nullptr)
				{
					std::weak_ptr<game::NodeComponent> parent = nodeComponentLock->GetParent();
					parentLock = parent.lock();
				}
			}

			if (parentLock != nullptr)
			{
				// TODO Find
			}
			else
			{
				_rootEntityNode._children.push_back(entityNode);
			}
		}
		*/
	}

	/// @brief 
	/// @param entity 
	void HierachyWindow::OnRemoveEntityCallback(std::weak_ptr<game::Entity> entity)
	{

	}

	/// @brief 
	/// @param entity 
	void HierachyWindow::OnRenameEntityCallback(std::weak_ptr<game::Entity> entity)
	{

	}

	/// @brief 
	/// @param componenent 
	void HierachyWindow::OnAddComponentCallback(std::weak_ptr<game::Component> componenent)
	{
		/*
		std::shared_ptr<game::Component> componentLock = componenent.lock();
		if (componentLock != nullptr)
		{
			if (componentLock->GetMetaType() == game::NodeComponent::GetMetaTypeStatic()) // todo or inherit !!!
			{
				std::shared_ptr<game::NodeComponent> nodeComponentLock = std::static_pointer_cast<game::NodeComponent>(componentLock);

				nodeComponentLock->GetParentChangeEvent() += _onParentChangeCallback;
			}
		}
		*/
	}

	/// @brief 
	/// @param componenent 
	void HierachyWindow::OnRemoveComponentCallback(std::weak_ptr<game::Component> componenent)
	{

	}

	/// @brief 
	void HierachyWindow::Draw()
	{
		game::World* world = game::World::GetInstance();
		for (auto& pair : world->GetEntities())
		{
			std::shared_ptr<game::Entity> entity = pair.second;
			std::shared_ptr<game::NodeComponent> nodeComponent = entity->GetComponent<game::NodeComponent>().lock();
			bool hasParent = false;
			if (nodeComponent != nullptr)
			{
				std::shared_ptr<game::NodeComponent> parentNodeComponent = nodeComponent->GetParent().Lock();
				if (parentNodeComponent != nullptr)
				{
					hasParent = true;
				}
			}

			if (hasParent == false)
			{
				DrawEntity(entity);
			}
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
			std::shared_ptr<game::Entity> selectionLock = _selection.lock();

			if (ImGui::MenuItem("Create Entity") == true)
			{
				std::weak_ptr<game::Entity> entity = world->CreateEntity("EditMe");

				if (selectionLock != nullptr)
				{
					std::shared_ptr<game::NodeComponent> selectionNodeComponentLock = selectionLock->GetComponent<game::NodeComponent>().lock();
					if (selectionNodeComponentLock != nullptr)
					{
						std::shared_ptr<game::Entity> entityLock = entity.lock();
						std::shared_ptr<game::NodeComponent> nodeComponentLock = entityLock->AddComponent<game::NodeComponent>().lock();
						nodeComponentLock->SetParent(selectionNodeComponentLock);
					}
				}

				ImGui::CloseCurrentPopup();
			}

			if (selectionLock != nullptr)
			{
				if (ImGui::MenuItem("Delete") == true)
				{
					world->DestroyEntity(selectionLock);
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
		ImGui::InvisibleButton("", ImVec2(200, 3));
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
					game::Entity::Id entityId = *reinterpret_cast<game::Entity::Id*>(payload->Data);
					game::World* world = game::World::GetInstance();
					std::weak_ptr<game::Entity> dropEntity = world->FindEntity(entityId);
					std::shared_ptr<game::Entity> dropEntityLock = dropEntity.lock();
					if (dropEntityLock != nullptr)
					{
						// todo get or add component
						std::shared_ptr<game::NodeComponent> parentNodeComponentLock;
						std::shared_ptr<game::NodeComponent> siblingNodeComponentLock = entityLock->GetComponent<game::NodeComponent>().lock();
						if (siblingNodeComponentLock != nullptr)
						{
							parentNodeComponentLock = siblingNodeComponentLock->GetParent().Lock();
						}

						// todo get or add component
						std::shared_ptr<game::NodeComponent> dropNodeComponentLock = dropEntityLock->GetComponent<game::NodeComponent>().lock();
						if (dropNodeComponentLock == nullptr)
						{
							dropNodeComponentLock = dropEntityLock->AddComponent<game::NodeComponent>().lock();
						}
						dropNodeComponentLock->SetParent(parentNodeComponentLock);
						// todo sibling index
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleVar();

		std::shared_ptr<game::Entity> selectionLock = _selection.lock();

		std::shared_ptr<game::NodeComponent> nodeComponent = entityLock->GetComponent<game::NodeComponent>().lock();

		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;
		if (nodeComponent == nullptr || nodeComponent->GetChildCount() == 0)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (entityLock == selectionLock)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(entityLock.get());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		bool opened = ImGui::TreeNodeEx(entityLock->GetName().c_str(), treeNodeFlags);
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			_selection = entityLock;
			Editor::GetInstance()->SetEntitySelection(_selection);
		}
		bool hovered = ImGui::IsItemHovered();
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (ImGui::IsWindowHovered() == true && hovered == true && ImGui::IsMouseReleased(ImGuiMouseButton_Right) == true)
		{
			_selection = entityLock;
			Editor::GetInstance()->SetEntitySelection(_selection);
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
			if (nodeComponent != nullptr)
			{
				uint32_t childCount = nodeComponent->GetChildCount();
				for (uint32_t childIndex = 0; childIndex < childCount; ++childIndex)
				{
					std::shared_ptr<game::NodeComponent> childNodeComponent = nodeComponent->GetChild(childIndex).Lock();
					if (childNodeComponent != nullptr)
					{
						DrawEntity(childNodeComponent->GetEntity());
					}
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
