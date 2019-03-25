#include "Component.h"

Component::Component(Actor* actor)
    : actor(actor)
{
}

Component::~Component()
{
}

Actor* Component::getActor() const
{
    return this->actor;
}
