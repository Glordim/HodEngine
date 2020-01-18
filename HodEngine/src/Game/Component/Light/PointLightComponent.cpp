#include "PointLightComponent.h"

namespace HOD
{
    namespace GAME
    {
        PointLightComponent::PointLightComponent(Actor* actor) : Component(actor)
        {
            _data.intensity = 1.0f;
            _data.range = 2.5f;
            _data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }

        void PointLightComponent::setupTweakBar(TwBar* bar)
        {

        }

        PointLight* PointLightComponent::GetPointLight() const
        {
            return (PointLight*)(&_data);
        }
    }
}
