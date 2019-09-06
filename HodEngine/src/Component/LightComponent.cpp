#include "LightComponent.h"

LightComponent::LightComponent(Actor* actor) : Component(actor)
{
    this->data.intensity = 1.0f;
    this->data.range = 2.5f;
    this->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

LightComponent::~LightComponent()
{
}

PointLight* LightComponent::GetPointLight() const
{
    return (PointLight*)(&this->data);
}
