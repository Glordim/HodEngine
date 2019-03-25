#include "LightComponent.h"

LightComponent::LightComponent(Actor* actor)
    : Component(actor)
    , intensity(1.0f)
    , range(2.5f)
    , color(1.0f, 1.0f, 1.0f, 1.0f)
{

}

LightComponent::~LightComponent()
{
}
