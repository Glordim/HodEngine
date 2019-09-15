#ifndef __RENDERER_COMPONENT_HPP__
#define __RENDERER_COMPONENT_HPP__

#include "Component.h"

class CameraComponent;
class PointLightComponent;

#include <vector>

class RendererComponent : public Component
{
public:
    RendererComponent() = delete;
    RendererComponent(const RendererComponent& copy) = delete;

    RendererComponent(Actor* actor);
    virtual ~RendererComponent();

    friend class Actor;
};

#endif
