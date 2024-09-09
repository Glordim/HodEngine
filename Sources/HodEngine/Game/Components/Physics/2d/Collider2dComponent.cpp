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
	DESCRIBE_REFLECTED_CLASS(Collider2dComponent, Component)
	{
		AddPropertyT(this, &Collider2dComponent::_isTrigger, "IsTrigger");
		AddPropertyT(this, &Collider2dComponent::_friction, "Friction");
		AddPropertyT(this, &Collider2dComponent::_density, "Density");
	}

	Collider2dComponent::Collider2dComponent()
	{
		
	}

	Collider2dComponent::~Collider2dComponent()
	{
		// collider owned by rigidoby
	}

	/// @brief 
	void Collider2dComponent::OnConstruct()
	{
	}

	/// @brief 
	void Collider2dComponent::OnAwake()
	{
		_rigidbody = GetEntity()->GetComponent<Rigidbody2dComponent>();
	}

	/// @brief 
	/// @return 
	std::shared_ptr<Rigidbody2dComponent> Collider2dComponent::GetRigidbody() const
	{
		return _rigidbody.lock();
	}
}
