#include "SceneComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <iostream>
#include <string>

#include <AntTweakBar.h>

#include "../Actor.h"

SceneComponent::SceneComponent(Actor* actor) : Component(actor)
{
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotation = glm::identity<glm::mat4x4>();
    this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    this->parent = nullptr;

    this->modelMatrixDirty = true;
}

SceneComponent::~SceneComponent()
{

}

void SceneComponent::setupTweakBar(TwBar* tweakBar)
{
    TwAddSeparator(tweakBar, "Scene", "");
    TwAddVarCB(tweakBar, "Position", TW_TYPE_DIR3F, &SceneComponent::twSetPos, &SceneComponent::twGetPos, static_cast<void*>(this), "");
    TwAddVarCB(tweakBar, "Rotation", TW_TYPE_QUAT4F, &SceneComponent::twSetRot, &SceneComponent::twGetRot, static_cast<void*>(this), "");
    TwAddVarRW(tweakBar, "Scale", TW_TYPE_DIR3F, &this->scale, "");
}

void SceneComponent::twGetPos(void *value, void *clientData)
{
    SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
    glm::vec3* pos = static_cast<glm::vec3*>(value);
    *pos = thiz->getPosition();
}

void SceneComponent::twSetPos(const void *value, void *clientData)
{
    SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
    thiz->setPosition(*(static_cast<const glm::vec3*>(value)));
}

void SceneComponent::twGetRot(void *value, void *clientData)
{
    SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
    glm::quat* pos = static_cast<glm::quat*>(value);
    *pos = thiz->getRotation();
}

void SceneComponent::twSetRot(const void *value, void *clientData)
{
    SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
    thiz->setRotation(*(static_cast<const glm::quat*>(value)));
}

void SceneComponent::lookAt(const glm::vec3& eye, const glm::vec3 target, const glm::vec3 up)
{
    this->modelMatrix = glm::lookAt(eye, target, up);

    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(this->modelMatrix, this->scale, this->rotation, this->position, skew, perspective);
    this->rotation = glm::conjugate(this->rotation);

    this->modelMatrixDirty = false;

    std::cout << std::to_string(this->position.x) << " " << std::to_string(this->position.y) << " " << std::to_string(this->position.z) << std::endl;
    std::cout << std::to_string(this->getRotationEuler().x) << " " << std::to_string(this->getRotationEuler().y) << " " << std::to_string(this->getRotationEuler().z) << std::endl;
}

glm::mat4 SceneComponent::getModelMatrix()
{
    if (this->modelMatrixDirty == true)
    {
        glm::mat4 pos = glm::translate(glm::identity<glm::mat4>(), this->position);
        glm::mat4 rot = glm::mat4_cast(this->rotation);
        glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), this->scale);

        this->modelMatrix = pos * rot * scale;
    }

    return this->modelMatrix;
}

void SceneComponent::setPosition(glm::vec3 position)
{
    this->position = position;
    this->modelMatrixDirty = true;

    this->syncPxActor();
}

glm::vec3 SceneComponent::getPosition() const
{
    return this->position;
}

void SceneComponent::rotate(float angle, glm::vec3 axis)
{
    this->rotation = glm::rotate(this->rotation, angle, axis);
    this->modelMatrixDirty = true;

    this->syncPxActor();
}

void SceneComponent::setRotation(glm::quat rot)
{
    this->rotation = rot;
    this->modelMatrixDirty = true;

    this->syncPxActor();
}

void SceneComponent::setRotation(glm::vec3 rot)
{
    rot.x = fmod(rot.x, 360.0f);
    rot.y = fmod(rot.y, 360.0f);
    rot.z = fmod(rot.z, 360.0f);

    this->rotation = glm::quat(radians(rot));
    this->modelMatrixDirty = true;

    this->syncPxActor();
}

glm::quat SceneComponent::getRotation() const
{
    return this->rotation;
}

glm::vec3 SceneComponent::getRotationEuler() const
{
    return glm::degrees(glm::eulerAngles(this->rotation));
}

size_t SceneComponent::getChildCount() const
{
    return this->childs.size();
}

SceneComponent* SceneComponent::getChild(size_t index)
{
    return this->childs[index];
}

void SceneComponent::setParent(SceneComponent* parent)
{
    // TODO retirer du vector childs du precedent parent

    this->parent = parent;
    this->parent->childs.push_back(this);
}

void SceneComponent::syncPxActor()
{
    physx::PxActor* pxActor = this->getActor()->getPxActor();

    if (pxActor != nullptr && (pxActor->getType() == physx::PxActorType::eRIGID_STATIC || pxActor->getType() == physx::PxActorType::eRIGID_DYNAMIC))
    {
        physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(pxActor);

        physx::PxTransform pxTransform(physx::PxVec3(this->position.x, this->position.y, this->position.z));
        pxTransform.q = physx::PxQuat(this->rotation.x, this->rotation.y, this->rotation.z, this->rotation.w);

        rigidActor->setGlobalPose(pxTransform);
    }
}