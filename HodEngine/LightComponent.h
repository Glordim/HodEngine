#ifndef __LIGHT_COMPONENT_HPP__
#define __LIGHT_COMPONENT_HPP__

#include "Component.h"
#include "Color.h"

class LightComponent : public Component
{
public:
    LightComponent() = delete;
    LightComponent(const LightComponent& copy) = delete;

    LightComponent(Actor* actor);
    virtual ~LightComponent();

    friend class Actor;

public:
    float intensity;
    float range;
    Color color;
};

#endif
