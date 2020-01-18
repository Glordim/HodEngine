#include "DirLightComponent.h"

namespace HOD
{
    namespace GAME
    {
        DirLightComponent::DirLightComponent(Actor* actor) : Component(actor)
        {
            _data.intensity = 1.0f;
            _data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }

        void DirLightComponent::setupTweakBar(TwBar* bar)
        {

        }

        DirLight* DirLightComponent::GetDirLight() const
        {
            return (DirLight*)(&_data);
        }
    }
}
