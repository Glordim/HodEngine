#ifndef __FLYING_POINT_LIGTH_HPP__
#define __FLYING_POINT_LIGTH_HPP__

#include "../Actor.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

class SceneComponent;

class FlyingPointLight : public Actor
{
public:
    FlyingPointLight() = delete;
    FlyingPointLight(const std::string& name, Scene* scene);
    FlyingPointLight(const FlyingPointLight& copy) = delete;
    virtual ~FlyingPointLight();

    virtual void start() override;
    virtual void update(float dt) override;

private:
    SceneComponent* sceneComponent;

    glm::vec3 initialPos;
};

#endif
