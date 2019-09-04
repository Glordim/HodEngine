#ifndef __FLYING_POINT_LIGTH_HPP__
#define __FLYING_POINT_LIGTH_HPP__

#include "../Actor.h"

#include "glm/vec3.hpp"

class SceneComponent;

class FlyingPointLight : public Actor
{
public:
    FlyingPointLight() = delete;
    FlyingPointLight(const std::string& name, Scene* scene);
    FlyingPointLight(const FlyingPointLight& copy) = delete;
    virtual ~FlyingPointLight();

    virtual void update(float dt) override;

private:
    SceneComponent* sceneComponent;
};

#endif
