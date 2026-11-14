#include "HodEngine/PhysicsComponents/Pch.hpp"
#include "HodEngine/PhysicsComponents/Components/Collider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Game/Scene.hpp"

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/Body.hpp>
#include <HodEngine/Physics/Collider.hpp>

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitAllowedClasses.hpp>
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp"
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline physics
{
	DESCRIBE_REFLECTED_CLASS(Collider2dComponent)
	{
		HOD_TRAIT(ReflectionTraitIcon, ICON_MDI_SHAPE_OUTLINE);
		// The base of the colliders, without a shape: not offered in the editor
		HOD_TRAIT(ReflectionTraitHide);

		// Not a Rigidbody2d: a collider without one is static (see TODO.txt)
		HOD_REQUIRED_COMPONENT(_node2d);

		HOD_PROPERTY(_isTrigger);
		HOD_PROPERTY(_friction);
		HOD_PROPERTY(_density);
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
		_rigidbody = GetComponentInParent<Rigidbody2dComponent>();
		if (_rigidbody == nullptr)
		{
			OUTPUT_ERROR("{} on '{}': no Rigidbody2d on the entity or its parents, the collider is ignored", GetReflectionDescriptorV().GetDisplayName(), GetOwner()->GetName());
		}
	}

	/// @brief
	/// @return
	Rigidbody2dComponent* Collider2dComponent::GetRigidbody()
	{
		if (_rigidbody == nullptr)
		{
			_rigidbody = GetComponentInParent<Rigidbody2dComponent>();
		}
		return _rigidbody.Get();
	}

	/// @brief 
	/// @return 
	Vector2 Collider2dComponent::GetScale() const
	{
		return _node2d->GetScale();
	}
}
