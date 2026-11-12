#include "HodEngine/PhysicsComponents/Pch.hpp"
#include "HodEngine/PhysicsComponents/Components/CircleCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Rigidbody2dComponent.hpp"

#include "HodEngine/Physics/Body.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCategory.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDescription.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp"
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline physics
{
	DESCRIBE_REFLECTED_CLASS(CircleCollider2dComponent, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_CIRCLE_OUTLINE);
		reflectionDescriptor.AddTrait<ReflectionTraitCategory>("Physics/Colliders");
		reflectionDescriptor.AddTrait<ReflectionTraitDescription>("Circular 2D collision shape");
		AddPropertyT(reflectionDescriptor, &CircleCollider2dComponent::_offset, "Offset", &CircleCollider2dComponent::SetOffset);
		AddPropertyT(reflectionDescriptor, &CircleCollider2dComponent::_radius, "Radius", &CircleCollider2dComponent::SetRadius);

		AddPropertyT(reflectionDescriptor, &CircleCollider2dComponent::_bounciness, "Bounciness"); // tmp
	}

	/// @brief 
	void CircleCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody == nullptr)
		{
			return;
		}
		Vector2 parentOffset = rigidbody->GetParentOffset(this);

		Vector2 scale = GetScale();
		_collider = rigidbody->GetInternalBody()->AddCircleShape(_isTrigger, parentOffset + _offset * scale, _radius * std::max(scale.GetX(), scale.GetY()));
		_collider->SetUserData(this);
		_collider->SetBounciness(_bounciness);
	}

	/// @brief 
	void CircleCollider2dComponent::SetOffset(const Vector2& offset)
	{
		_offset = offset;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsCircleShape(parentOffset + _offset * scale, _radius * std::max(scale.GetX(), scale.GetY()));
		}
	}

	/// @brief 
	/// @return 
	const Vector2& CircleCollider2dComponent::GetOffset() const
	{
		return _offset;
	}

	/// @brief 
	void CircleCollider2dComponent::SetRadius(float radius)
	{
		_radius = radius;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsCircleShape(parentOffset + _offset * scale, _radius * std::max(scale.GetX(), scale.GetY()));
		}
	}

	/// @brief 
	/// @return 
	float CircleCollider2dComponent::GetRadius() const
	{
		return _radius;
	}
}
