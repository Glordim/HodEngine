#include "ActorDebugWindow.h"

#include "../Actor.h"

#include <ImGui/src/imgui.h>

namespace HOD
{
    namespace GAME
    {
		ActorDebugWindow::ActorDebugWindow() : DebugWindow()
        {

        }

		ActorDebugWindow::~ActorDebugWindow()
		{
            
        }

        void ActorDebugWindow::Draw()
        {
            ImGui::Begin("Selected Actor");
            {
                if (_pActor == nullptr)
                {
                    ImGui::Text("No selected actor...");
                }
                else
                {
                    ImGui::Separator();
                    ImGui::Text("Name: %s", _pActor->getName().c_str());
                    ImGui::Separator();

					_pActor->DrawImGui();

                    std::vector<GAME::Component*> vComponent = _pActor->getAllComponent();
                    for (GAME::Component* pComponent : vComponent)
                    {
                        if (ImGui::CollapsingHeader(pComponent->GetName(), _actorComponentCollapseMap[pComponent->GetName()]) == true)
                        {
                            pComponent->DrawImGui();
                        }
                    }
                }
            }
            ImGui::End();
        }

        void ActorDebugWindow::SetActor(Actor* pActor)
        {
            _pActor = pActor;
        }
    }
}
