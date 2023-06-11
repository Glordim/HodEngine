#include "HodEngine/Editor/HierachyWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Game/src/World.h"
#include "HodEngine/Game/src/Scene.h"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(HierachyWindow, "Hierachy", true)

	/// @brief 
	void HierachyWindow::Draw()
	{
		game::World* world = game::World::GetInstance();

		for (game::Scene* scene : world->GetScenes())
		{
			if (ImGui::CollapsingHeader(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen) == true)
			{
				if (ImGui::BeginPopupContextItem("ContextPopup") == true)
				{
					if (ImGui::MenuItem("Create GameObject") == true)
					{
						
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
	}
}
