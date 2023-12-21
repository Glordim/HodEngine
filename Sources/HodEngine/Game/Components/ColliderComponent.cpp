#include "HodEngine/Game/Components/ColliderComponent.hpp"

#include "HodEngine/Game/Scene.hpp"

#include <HodEngine/Physics/Actor.hpp>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
			/*
		ColliderComponent::ColliderComponent(const std::weak_ptr<Entity>& entity) : Component(entity)
		{
			Scene* scene = actor->GetScene();

			physics::Actor* physicActor = actor->GetPhysicActor();

			if (physicActor == nullptr)
			{
				glm::vec2 position = actor->GetComponent<SceneComponent>()->GetPosition();
				float rotation = actor->GetComponent<SceneComponent>()->GetRotation();
				glm::vec2 scale = glm::vec2(1.0f, 1.0f);

				physicActor = scene->CreatePhysicActor(actor);
				physicActor->SetTransform(position, rotation, scale);
			}
		}
			*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition)
		{
			//GetActor()->GetPhysicActor()->AddEdgeShape(startPosition, endPosition);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddCircleShape(const Vector2& position, float radius)
		{
			//GetActor()->GetPhysicActor()->AddCircleShape(position, radius);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density)
		{
			//GetActor()->GetPhysicActor()->AddBoxShape(position, size, angle, density);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddConvexShape(const std::vector<const Vector2>& vertices)
		{
			//GetActor()->GetPhysicActor()->AddConvexShape(vertices);
		}
	}
}
