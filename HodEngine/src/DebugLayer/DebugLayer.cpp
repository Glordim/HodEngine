#include "DebugLayer.h"

#include "ImGui/imgui.h"

#include "Game/Actor.h"

namespace HOD
{
    DEBUG_LAYER::DebugLayer* CORE::Singleton<DEBUG_LAYER::DebugLayer>::_instance = nullptr;

    namespace DEBUG_LAYER
    {
        DebugLayer::DebugLayer() : Singleton()
        {

        }

        DebugLayer::~DebugLayer()
        {
            
        }

        bool DebugLayer::Init()
        {
            return true;
        }

        void DebugLayer::Clear()
        {

        }

        void DebugLayer::Draw()
        {
            ImGui::Begin("Selected Actor");
            {
                if (pActor == nullptr)
                {
                    ImGui::Text("No selected actor...");
                }
                else
                {
                    ImGui::Separator();
                    ImGui::Text("Name: %s", pActor->getName().c_str());
                    ImGui::Separator();

                    pActor->DrawImGui();

                    std::vector<GAME::Component*> vComponent = pActor->getAllComponent();
                    for (GAME::Component* pComponent : vComponent)
                    {
                        if (ImGui::CollapsingHeader(pComponent->GetName(), actorComponentCollapseMap[pComponent->GetName()]) == true)
                        {
                            pComponent->DrawImGui();
                        }
                    }
                }
            }
            ImGui::End();
        }

        void DebugLayer::ShowActor(GAME::Actor* pActor)
        {
            this->pActor = pActor;
        }
    }
}
