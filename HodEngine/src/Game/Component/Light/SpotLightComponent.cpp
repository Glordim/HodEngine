#include "SpotLightComponent.h"

namespace HOD
{
    namespace GAME
    {
        SpotLightComponent::SpotLightComponent(Actor* actor) : Component(actor)
        {
            _data.intensity = 1.0f;
            _data.radius = 2.5f;
            _data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }

        void SpotLightComponent::setupTweakBar(TwBar* bar)
        {

        }

        SpotLight* SpotLightComponent::GetSpotLight() const
        {
            return (SpotLight*)(&_data);
        }
    }
}
