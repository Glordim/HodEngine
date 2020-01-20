#include "ColliderComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

#include "../Actor.h"
#include "../Scene.h"

#include "PxPhysicsAPI.h"

namespace HOD
{
    namespace GAME
    {
        ColliderComponent::ColliderComponent(Actor* actor) : Component(actor)
        {

        }

        void ColliderComponent::setupTweakBar(TwBar* bar)
        {

        }

        void ColliderComponent::SetShape(ColliderComponent::Shape shape)
        {
            Actor* actor = GetActor();
            Scene* scene = actor->getScene();

             = scene->getDefaultMaterial();

            physx::PxActor* pxActor = actor->getPxActor();
			physx::PxMaterial* pxMaterial = pxActor->getScene()->getDefaultMaterial();

            if (pxActor == nullptr)
            {
                glm::vec3 position = actor->getComponent<SceneComponent>()->getPosition();
                glm::quat rotation = actor->getComponent<SceneComponent>()->getRotation();

				pxActor = scene->CreatePhysicActor(actor);
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
    }
}
