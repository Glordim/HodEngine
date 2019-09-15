#ifndef __SPOT_LIGHT_COMPONENT_HPP__
#define __SPOT_LIGHT_COMPONENT_HPP__

#include "../Component.h"
#include "../../Color.h"

#include "../../Renderer/Light/SpotLight.h"

class SpotLightComponent : public Component
{
public:
    SpotLightComponent() = delete;
    SpotLightComponent(const SpotLightComponent& copy) = delete;

    SpotLightComponent(Actor* actor);
    virtual ~SpotLightComponent();

    SpotLight* GetSpotLight() const;

    friend class Actor;

public:
    SpotLight data;
};

#endif
