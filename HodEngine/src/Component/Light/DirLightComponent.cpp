#include "DirLightComponent.h"

DirLightComponent::DirLightComponent(Actor* actor) : Component(actor)
{
    this->data.intensity = 1.0f;
    this->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

DirLightComponent::~DirLightComponent()
{
}

DirLight* DirLightComponent::GetDirLight() const
{
    return (DirLight*)(&this->data);
}
