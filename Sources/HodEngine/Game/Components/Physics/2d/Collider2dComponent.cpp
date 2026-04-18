#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Game/Scene.hpp"

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/Body.hpp>
#include <HodEngine/Physics/Collider.hpp>

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitAllowedClasses.hpp>

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(Collider2dComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Collider2dComponent::_isTrigger, "IsTrigger");
		AddPropertyT(reflectionDescriptor, &Collider2dComponent::_friction, "Friction");
		AddPropertyT(reflectionDescriptor, &Collider2dComponent::_density, "Density");
	}

	Collider2dComponent::Collider2dComponent()
	{
		
	}

	Collider2dComponent::~Collider2dComponent()
	{
		// collider owned by rigidoby
	}

	/// @brief 
	void Collider2dComponent::OnStart()
	{
		_rigidbody = GetOwner()->GetComponentInParent<Rigidbody2dComponent>();
	}

	/// @brief 
	/// @return 
	Rigidbody2dComponent* Collider2dComponent::GetRigidbody()
	{
		if (_rigidbody == nullptr)
		{
			_rigidbody = GetOwner()->GetComponentInParent<Rigidbody2dComponent>();
		}
		return _rigidbody.Get();
	}

	/// @brief 
	/// @return 
	Vector2 Collider2dComponent::GetScale() const
	{
		Entity* entity = GetOwner();
		if (entity != nullptr)
		{
			Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
			if (node2dComponent != nullptr)
			{
				return node2dComponent->GetScale();
			}
		}
		return Vector2::One;
	}
}
