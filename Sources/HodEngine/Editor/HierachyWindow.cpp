#include "HodEngine/Editor/HierachyWindow.h"
#include "HodEngine/Editor/Editor.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Game/src/World.h"
#include "HodEngine/Game/src/Scene.h"
#include "HodEngine/Game/src/Actor.h"

#include "HodEngine/Game/src/Components/NodeComponent.h"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(HierachyWindow, "Hierachy", true)

	/// @brief 
	HierachyWindow::HierachyWindow()
	: _onNewEntityCallback(std::bind(&HierachyWindow::OnNewEntityCallback, this, std::placeholders::_1))
	, _onRemoveEntityCallback(std::bind(&HierachyWindow::OnRemoveEntityCallback, this, std::placeholders::_1))
	, _onRenameEntityCallback(std::bind(&HierachyWindow::OnRenameEntityCallback, this, std::placeholders::_1))
	{
		game::World* world = game::World::GetInstance();

		world->GetNewEntityEvent() += _onNewEntityCallback;
		world->GetRemoveEntityEvent() += _onRemoveEntityCallback;
		world->GetRenameEntityEvent() += _onRenameEntityCallback;
	}

	/// @brief 
	/// @param entity 
	void HierachyWindow::OnNewEntityCallback(std::weak_ptr<game::Entity> entity)
	{
		std::shared_ptr<game::Entity> entityLock = entity.lock();
		if (entityLock != nullptr)
		{
			EntityNode* entityNode = new EntityNode();
			entityNode->_entity = entity;

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
	void HierachyWindow::Draw()
	{
		for (EntityNode* entityNode : _rootEntityNode._children)
		{
			DrawEntityNode(entityNode);
		}
	}

	/// @brief 
	/// @param entityNode 
	void HierachyWindow::DrawEntityNode(EntityNode* entityNode)
	{
		std::shared_ptr<game::Entity> selectionLock = _selection.lock();

		std::shared_ptr<game::Entity> entityLock = entityNode->_entity.lock();

		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf;
		if (entityLock == selectionLock)
		{
			treeNodeFlags|= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(entityLock.get());
		bool opened = ImGui::TreeNodeEx(entityLock->GetName().c_str(), treeNodeFlags);
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			_selection = entityLock;
			Editor::GetInstance()->SetEntitySelection(_selection);
		}
		ImGui::PopID();
		if (opened == true)
		{
			ImGui::TreePop();
		}

		if (ImGui::BeginPopupContextItem("ContextPopup") == true)
		{
			if (ImGui::MenuItem("Create GameObject") == true)
			{
				game::World* world = game::World::GetInstance();
				std::weak_ptr<game::Entity> entity = world->CreateEntity("EditMe");
			}
			/*
			if (ImGui::MenuItem("Create GameObject") == true)
			{
				
			}
			*/
			ImGui::EndPopup();
		}
/*
		for (game::Scene* scene : world->GetScenes())
		{
			if (ImGui::CollapsingHeader(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen) == true)
			{
				for (game::Actor* actor : scene->GetActors())
				{
					ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf;
					if (actor == _selection)
					{
						treeNodeFlags|= ImGuiTreeNodeFlags_Selected;
					}

					ImGui::PushID(actor);
					bool opened = ImGui::TreeNodeEx(actor->GetName().c_str(), treeNodeFlags);
					if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
					{
						_selection = actor;
						Editor::GetInstance()->SetActorSelection(_selection);
					}
					ImGui::PopID();
					if (opened == true)
					{
						ImGui::TreePop();
					}
				}

				if (ImGui::BeginPopupContextItem("ContextPopup") == true)
				{
					if (ImGui::MenuItem("Create GameObject") == true)
					{
						game::Actor* actor = scene->SpawnActor<game::Actor>("EditMe");
					}
					ImGui::EndPopup();
				}
			}
		}
*/
	}
}
