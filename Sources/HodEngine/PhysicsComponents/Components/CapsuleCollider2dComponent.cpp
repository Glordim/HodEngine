#include "HodEngine/PhysicsComponents/Pch.hpp"
#include "HodEngine/PhysicsComponents/Components/CapsuleCollider2dComponent.hpp"
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
	DESCRIBE_REFLECTED_CLASS(CapsuleCollider2dComponent)
	{
		HOD_TRAIT(ReflectionTraitIcon, ICON_MDI_PILL);
		HOD_TRAIT(ReflectionTraitCategory, "Physics/Colliders");
		HOD_TRAIT(ReflectionTraitDescription, "Capsule-shaped 2D collision shape");

		HOD_PROPERTY(_offset, &CapsuleCollider2dComponent::SetOffset);
		HOD_PROPERTY(_height, &CapsuleCollider2dComponent::SetHeight);
		HOD_PROPERTY(_radius, &CapsuleCollider2dComponent::SetRadius);
		HOD_PROPERTY(_rotation, &CapsuleCollider2dComponent::SetRotation);
	}

	/// @brief 
	void CapsuleCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody == nullptr)
		{
			return;
		}
		Vector2 parentOffset = rigidbody->GetParentOffset(this);

		Vector2 scale = GetScale();
		_collider = rigidbody->GetInternalBody()->AddCapsuleShape(_isTrigger, parentOffset + _offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
		_collider->SetUserData(this);
	}

	/// @brief 
	void CapsuleCollider2dComponent::SetOffset(const Vector2& offset)
	{
		_offset = offset;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(parentOffset + _offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& CapsuleCollider2dComponent::GetOffset() const
	{
		return _offset;
	}

	/// @brief 
	void CapsuleCollider2dComponent::SetHeight(float height)
	{
		_height = height;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(parentOffset + _offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
		}
	}

	/// @brief 
	/// @return 
	float CapsuleCollider2dComponent::GetHeight() const
	{
		return _height;
	}

	/// @brief 
	void CapsuleCollider2dComponent::SetRadius(float radius)
	{
		_radius = radius;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(parentOffset + _offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
		}
	}

	/// @brief 
	/// @return 
	float CapsuleCollider2dComponent::GetRadius() const
	{
		return _radius;
	}

	/// @brief 
	void CapsuleCollider2dComponent::SetRotation(float rotation)
	{
		_rotation = rotation;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(parentOffset + _offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
		}
	}

	/// @brief 
	/// @return 
	float CapsuleCollider2dComponent::GetRotation() const
	{
		return _rotation;
	}
}
