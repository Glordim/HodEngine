#ifndef __POINT_LIGHT_COMPONENT_HPP__
#define __POINT_LIGHT_COMPONENT_HPP__

#include "../Component.h"
#include "../../Color.h"

#include "../../Renderer/Light/PointLight.h"

class PointLightComponent : public Component
{
public:
    PointLightComponent() = delete;
    PointLightComponent(const PointLightComponent& copy) = delete;

    PointLightComponent(Actor* actor);
    virtual ~PointLightComponent();

    PointLight* GetPointLight() const;

    friend class Actor;

public:
    PointLight data;
};

#endif
