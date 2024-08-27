#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/ColliderComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Game/Scene.hpp"

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/Body.hpp>

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitAllowedClasses.hpp>

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS_NO_PARENT(Shape)
		{
			
		}

		DESCRIBE_REFLECTED_CLASS(EdgeShape, Shape)
		{
			/*
			AddPropertyT(this, &EdgeShape::_start);
			AddPropertyT(this, &EdgeShape::_end);
			*/
		}

		DESCRIBE_REFLECTED_CLASS(CircleShape, Shape)
		{
			/*
			AddPropertyT(this, &CircleShape::_origin);
			AddPropertyT(this, &CircleShape::_radius);
			*/
		}

		DESCRIBE_REFLECTED_CLASS(BoxShape, Shape)
		{
			/*
			AddPropertyT(this, &BoxShape::_origin);
			AddPropertyT(this, &BoxShape::_size);
			AddPropertyT(this, &BoxShape::_angle);
			*/
		}

		DESCRIBE_REFLECTED_CLASS(ColliderComponent, Component)
		{
			ReflectionProperty* shapesProperty = AddPropertyT(this, &ColliderComponent::_shapes, "_shapes");
			ReflectionTraitAllowedClasses* traitAllowedClasses = shapesProperty->AddTrait<ReflectionTraitAllowedClasses>();
			traitAllowedClasses->AddAllowedClass<EdgeShape>();
			traitAllowedClasses->AddAllowedClass<CircleShape>();
			traitAllowedClasses->AddAllowedClass<BoxShape>();

			AddPropertyT(this, &ColliderComponent::_dynamic, "_dynamic");
		}

		ColliderComponent::ColliderComponent()
		{
			_body = physics::Physics::GetInstance()->CreateBody();
			_body->AddBoxShape(Vector2::Zero, Vector2::One * 0.5f, 0.0f, 1.0f); // TODO tmp
		}

		ColliderComponent::~ColliderComponent()
		{
			physics::Physics::GetInstance()->DeleteBody(_body);
		}

		/// @brief 
		void ColliderComponent::OnConstruct()
		{
		}

		/// @brief 
		void ColliderComponent::OnAwake()
		{
			if (_dynamic)
			{
				_body->SetType(physics::Body::Type::Dynamic);
			}

			std::shared_ptr<Entity> entity = GetEntity();
			if (entity != nullptr)
			{
				std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					_body->SetTransform(node2dComponent->GetPosition(), node2dComponent->GetRotation(), node2dComponent->GetScale());
				}
			}
		}

		/// @brief 
		void ColliderComponent::OnUpdate()
		{
			std::shared_ptr<Entity> entity = GetEntity();
			if (entity != nullptr)
			{
				std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					node2dComponent->SetPosition(_body->GetPosition());
					node2dComponent->SetRotation(_body->GetRotation());
				}
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition)
		{
			_body->AddEdgeShape(startPosition, endPosition);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddCircleShape(const Vector2& position, float radius)
		{
			_body->AddCircleShape(position, radius);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density)
		{
			_body->AddBoxShape(position, size, angle, density);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void ColliderComponent::AddConvexShape(const std::vector<const Vector2>& vertices)
		{
			_body->AddConvexShape(vertices);
		}
	}
}
