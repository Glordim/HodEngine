#ifndef __DIR_LIGHT_COMPONENT_HPP__
#define __DIR_LIGHT_COMPONENT_HPP__

#include "../Component.h"
#include "../../Color.h"

#include "../../Renderer/Light/DirLight.h"

class DirLightComponent : public Component
{
public:
    DirLightComponent() = delete;
    DirLightComponent(const DirLightComponent& copy) = delete;

    DirLightComponent(Actor* actor);
    virtual ~DirLightComponent();

    DirLight* GetDirLight() const;

    friend class Actor;

public:
    DirLight data;
};

#endif
