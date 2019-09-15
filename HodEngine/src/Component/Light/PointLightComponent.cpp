#include "PointLightComponent.h"

PointLightComponent::PointLightComponent(Actor* actor) : Component(actor)
{
    this->data.intensity = 1.0f;
    this->data.range = 2.5f;
    this->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

PointLightComponent::~PointLightComponent()
{
}

PointLight* PointLightComponent::GetPointLight() const
{
    return (PointLight*)(&this->data);
}
