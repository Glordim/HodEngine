#include "RendererComponent.h"

namespace HOD
{
    namespace GAME
    {
        RendererComponent::RendererComponent(Actor* actor) : Component(actor)
        {

        }

        void RendererComponent::DrawImGui()
        {

        }

        const char* RendererComponent::GetName()
        {
            return "Renderer";
        }
    }
}
