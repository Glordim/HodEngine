#include "ColliderComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

#include "../Actor.h"
#include "../Scene.hpp"

#include "PxPhysicsAPI.h"

ColliderComponent::ColliderComponent(Actor* actor) : Component(actor)
{
    
}

ColliderComponent::~ColliderComponent()
{

}

void ColliderComponent::setShape(ColliderComponent::Shape shape)
{
    Actor* actor = this->getActor();
    Scene* scene = actor->getScene();

    physx::PxMaterial* pxMaterial = scene->getDefaultMaterial();

    physx::PxActor* pxActor = actor->getPxActor();

    if (pxActor == nullptr)
    {
        glm::vec3 position = actor->getComponent<SceneComponent>()->getPosition();
        glm::quat rotation = actor->getComponent<SceneComponent>()->getRotation();

        physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));
        pxTransform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

        pxActor = PxGetPhysics().createRigidStatic(pxTransform);

        scene->addPxActor(actor, pxActor);
    }

    if (pxActor->getType() == physx::PxActorType::eRIGID_STATIC || pxActor->getType() == physx::PxActorType::eRIGID_DYNAMIC)
    {
        physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(pxActor);

        physx::PxShape* pxShape = PxGetPhysics().createShape(physx::PxBoxGeometry(0.1f, 0.1f, 0.1f), *pxMaterial);
        pxShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
        rigidActor->attachShape(*pxShape);
        rigidActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
    }    
}
