#include "HodEngine/Editor/HierachyWindow.h"
#include "HodEngine/Editor/Editor.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Game/src/World.h"
#include "HodEngine/Game/src/Scene.h"
#include "HodEngine/Game/src/Actor.h"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(HierachyWindow, "Hierachy", true)

	/// @brief 
	void HierachyWindow::Draw()
	{
		game::World* world = game::World::GetInstance();

		std::shared_ptr<game::Entity> selectionLock = _selection.lock();

		for (const std::weak_ptr<game::Entity>& entity : world->GetEntities())
		{
			std::shared_ptr<game::Entity> entityLock = entity.lock();

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
		}

		if (ImGui::BeginPopupContextItem("ContextPopup") == true)
		{
			if (ImGui::MenuItem("Create GameObject") == true)
			{
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
					/*
					if (ImGui::MenuItem("Create GameObject") == true)
					{
						
					}
					*/
					ImGui::EndPopup();
				}
			}
		}
*/
	}
}
