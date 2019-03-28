#include "ColliderComponent.h"

#include "glm/vec3.hpp"

#include "Actor.h"
#include "Scene.hpp"

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

    glm::vec3 position = actor->getComponent<SceneComponent>()->getPosition();
    glm::quat rotation = actor->getComponent<SceneComponent>()->getRotation();

    physx::PxMaterial* pxMaterial = scene->getDefaultMaterial();

    physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));
    pxTransform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

    physx::PxRigidStatic* pxActor = PxGetPhysics().createRigidStatic(pxTransform);

    physx::PxShape* pxShape = PxGetPhysics().createShape(physx::PxBoxGeometry(0.1f, 0.1f, 0.1f), *pxMaterial);
    pxShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
    pxActor->attachShape(*pxShape);
    pxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);

    scene->addPxActor(actor, pxActor);
}
