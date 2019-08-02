#ifndef __CAMERA_COMPONENT_HPP__
#define __CAMERA_COMPONENT_HPP__

#include <vector>

#include "Component.h"

#include "glm/mat4x4.hpp"

#include "Scene.hpp"

class LightComponent;

class CameraComponent : public Component
{
public:
    CameraComponent() = delete;
    CameraComponent(const CameraComponent& copy) = delete;

    CameraComponent(Actor* actor);
    virtual ~CameraComponent();

public:

    virtual void setupTweakBar(TwBar* tweakBar) override;

    void render(Scene& scene);

    const glm::mat4& getProjectionMatrix();

private:
    void drawSceneComponent(SceneComponent* sceneComponent, std::vector<LightComponent*>& allLight);

private:
    float fov;
    float aspect;
    float fNear;
    float fFar;
    bool perspective;
    bool dirtyFlag;

    glm::mat4 projectionMatrix;
};

#endif

