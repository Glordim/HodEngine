#ifndef __SCENE_COMPONENT_HPP__
#define __SCENE_COMPONENT_HPP__

#include "Component.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/quaternion.hpp"

#include <vector>

class SceneComponent : public Component
{
public:
    SceneComponent() = delete;
    SceneComponent(const SceneComponent& copy) = delete;

    SceneComponent(Actor* actor);
    virtual ~SceneComponent();

    friend class Actor;

public:

    size_t getChildCount() const;
    SceneComponent* getChild(int index);
    void setParent(SceneComponent* parent);

    void lookAt(const glm::vec3& eye, const glm::vec3 target, const glm::vec3 up);

    glm::mat4 getModelMatrix();

    void setPosition(glm::vec3 position);
    glm::vec3 getPosition() const;

    void setRotation(glm::quat rot);
    void setRotation(glm::vec3 rotation);
    glm::quat getRotation() const;
    glm::vec3 getRotationEuler() const;
    void rotate(float angle, glm::vec3 axis);

    glm::vec3 position;
    glm::vec3 scale;

    void setupTweakBar(TwBar* tweakBar);

private:
    static void twSetPos(const void *value, void *clientData);
    static void twGetPos(void *value, void *clientData);

    static void twSetRot(const void *value, void *clientData);
    static void twGetRot(void *value, void *clientData);

    void syncPxActor();

    bool modelMatrixDirty;
    glm::mat4 modelMatrix;

    glm::quat rotation;

    std::vector<SceneComponent*> childs;
    SceneComponent* parent;
};

#endif
