#ifndef __CAMERA_COMPONENT_HPP__
#define __CAMERA_COMPONENT_HPP__

#include <vector>

#include "Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include "../Scene.hpp"

class MaterialInstance;
class PointLightComponent;

class CameraComponent : public Component
{
public:
    CameraComponent() = delete;
    CameraComponent(const CameraComponent& copy) = delete;

    CameraComponent(Actor* actor);
    virtual ~CameraComponent();

    virtual void setupTweakBar(TwBar* tweakBar) override;

    void render(Scene& scene);

    const glm::mat4& getProjectionMatrix();

    void SetHdriMaterial(MaterialInstance* hdriMat);

private:
    float fov;
    float aspect;
    float fNear;
    float fFar;
    bool perspective;
    bool dirtyFlag;

    MaterialInstance* hdriMat;

    glm::mat4 projectionMatrix;
};

#endif

