#ifndef __LIGHT_COMPONENT_HPP__
#define __LIGHT_COMPONENT_HPP__

#include "Component.h"
#include "../Color.h"

#include "../Renderer/PointLight.h"

class LightComponent : public Component
{
public:
    LightComponent() = delete;
    LightComponent(const LightComponent& copy) = delete;

    LightComponent(Actor* actor);
    virtual ~LightComponent();

    PointLight* GetPointLight() const;

    friend class Actor;

public:
    PointLight data;
};

#endif
