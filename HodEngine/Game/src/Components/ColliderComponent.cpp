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
				glm::vec2 scale = glm::vec2(1.0f, 1.0f);

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
		void ColliderComponent::AddEdgeShape(const glm::vec2& startPosition, const glm::vec2& endPosition)
		{
			GetActor()->GetPhysicActor()->AddEdgeShape(startPosition, endPosition);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddCircleShape(const glm::vec2& position, float radius)
		{
			GetActor()->GetPhysicActor()->AddCircleShape(position, radius);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddBoxShape(const glm::vec2& position, const glm::vec2& size, float angle, float density)
		{
			GetActor()->GetPhysicActor()->AddBoxShape(position, size, angle, density);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddConvexShape(const std::vector<const glm::vec2>& vertices)
		{
			GetActor()->GetPhysicActor()->AddConvexShape(vertices);
		}
	}
}
