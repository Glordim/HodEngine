#include "ActorDebugWindow.h"

#include "../Actor.h"

#include <HodEngine/ImGui/src/imgui.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		ActorDebugWindow::ActorDebugWindow() : DebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		ActorDebugWindow::~ActorDebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ActorDebugWindow::Draw()
		{
			ImGui::Begin("Selected Actor");
			{
				if (_actor == nullptr)
				{
					ImGui::Text("No selected actor...");
				}
				else
				{
					ImGui::Separator();
					ImGui::Text("Name: %s", _actor->GetName().c_str());
					ImGui::Separator();

					_actor->DrawImGui();

					std::vector<GAME::Component*> vComponent = _actor->GetAllComponent();
					for (GAME::Component* pComponent : vComponent)
					{
						if (ImGui::CollapsingHeader(pComponent->GetType(), _actorComponentCollapseMap[pComponent->GetType()]) == true)
						{
							pComponent->DrawImGui();
						}
					}
				}
			}
			ImGui::End();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ActorDebugWindow::SetActor(Actor* actor)
		{
			_actor = actor;
		}
	}
}
