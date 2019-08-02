#ifndef __COLLIDER_COMPONENT_HPP__
#define __COLLIDER_COMPONENT_HPP__

#include "Component.h"

class ColliderComponent : public Component
{
public:
    enum Shape
    {
        Box,
        Sphere,
        Mesh
    };

public:
    ColliderComponent() = delete;
    ColliderComponent(const ColliderComponent& copy) = delete;

    ColliderComponent(Actor* actor);
    virtual ~ColliderComponent();

    friend class Actor;

    void setShape(Shape shape);
};

#endif // __COLLIDER_COMPONENT_HPP__
