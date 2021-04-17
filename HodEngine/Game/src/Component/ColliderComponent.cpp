#include "ColliderComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

#include "../Actor.h"
#include "../Scene.h"

#include <Physics/src/Actor.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		ColliderComponent::ColliderComponent(Actor* actor) : Component(actor)
		{
			Scene* scene = actor->GetScene();

			PHYSICS::Actor* physicActor = actor->GetPhysicActor();

			if (physicActor == nullptr)
			{
				glm::vec2 position = actor->GetComponent<SceneComponent>()->GetPosition();
				float rotation = actor->GetComponent<SceneComponent>()->GetRotation();
				glm::vec2 scale = glm::vec3(1.0f, 1.0f, 1.0f);

				physicActor = scene->CreatePhysicActor(actor);
				physicActor->SetTransform(position, rotation, scale);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::DrawImGui()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* ColliderComponent::GetType() const
		{
			return "Collider";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::SetShape(PHYSICS::SHAPE eShape)
		{
			Actor* actor = GetActor();
			PHYSICS::Actor* physicActor = actor->GetPhysicActor();
/*
			StaticMeshComponent* pStaticMeshComponent = actor->GetComponent<StaticMeshComponent>();
			SceneComponent* pSceneComponent = actor->GetComponent<SceneComponent>();

			if (pStaticMeshComponent != nullptr && pStaticMeshComponent->GetMesh() != nullptr)
			{
				physicActor->SetShape(eShape, pStaticMeshComponent->GetMesh()->GetBoundingBox(), pSceneComponent->GetScale());
			}
			else
			{
				physicActor->SetShape(eShape, RENDERER::BoundingBox(), pSceneComponent->GetScale());
			}
			*/
		}
	}
}
