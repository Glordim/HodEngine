#include "Actor.h"

#include "AntTweakBar.h"

Actor::Actor(const std::string& name, Scene* scene)
    : name(name)
    , scene(scene)
{

}

Actor::~Actor()
{
}

void Actor::setupInputListener(InputListener* inputListener)
{

}

void Actor::update(float dt)
{

}

void Actor::setupTweakBarForAllComponent(TwBar* twBar)
{
    TwRemoveAllVars(twBar);

    auto it = this->componentMapping.begin();
    auto itEnd = this->componentMapping.end();

    while (it != itEnd)
    {
        it->second->setupTweakBar(twBar);

        ++it;
    }
}
