#include "ColliderComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

#include "../Actor.h"
#include "../Scene.h"

#include <Physic/src/Actor.h>

namespace HOD
{
    namespace GAME
    {
        ColliderComponent::ColliderComponent(Actor* actor) : Component(actor)
        {
            Scene* scene = actor->getScene();

            PHYSIC::Actor* physicActor = actor->GetPhysicActor();

            if (physicActor == nullptr)
            {
                glm::vec3 position = actor->getComponent<SceneComponent>()->getPosition();
                glm::quat rotation = actor->getComponent<SceneComponent>()->getRotation();
                glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

                physicActor = scene->CreatePhysicActor(actor);
                physicActor->SetTransform(position, rotation, scale);
            }
        }

        void ColliderComponent::DrawImGui()
        {

        }

        const char* ColliderComponent::GetName()
        {
            return "Collider";
        }

        void ColliderComponent::SetShape(PHYSIC::SHAPE eShape)
        {
            Actor* actor = GetActor();

            PHYSIC::Actor* physicActor = actor->GetPhysicActor();

            physicActor->SetShape(eShape);
        }
    }
}
