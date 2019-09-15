#include "SpotLightComponent.h"

SpotLightComponent::SpotLightComponent(Actor* actor) : Component(actor)
{
    this->data.intensity = 1.0f;
    this->data.radius = 2.5f;
    this->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

SpotLightComponent::~SpotLightComponent()
{
}

SpotLight* SpotLightComponent::GetSpotLight() const
{
    return (SpotLight*)(&this->data);
}
