#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/CapsuleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"

#include "HodEngine/Physics/Body.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(CapsuleCollider2dComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CapsuleCollider2dComponent::_offset, "Offset", &CapsuleCollider2dComponent::SetOffset);
		AddPropertyT(reflectionDescriptor, &CapsuleCollider2dComponent::_height, "Height", &CapsuleCollider2dComponent::SetHeight);
		AddPropertyT(reflectionDescriptor, &CapsuleCollider2dComponent::_radius, "Radius", &CapsuleCollider2dComponent::SetRadius);
		AddPropertyT(reflectionDescriptor, &CapsuleCollider2dComponent::_rotation, "Rotation", &CapsuleCollider2dComponent::SetRotation);
	}

	/// @brief 
	void CapsuleCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Vector2 scale = GetScale();
		_collider = GetRigidbody()->GetInternalBody()->AddCapsuleShape(_isTrigger, _offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
	}

	/// @brief 
	void CapsuleCollider2dComponent::SetOffset(const Vector2& offset)
	{
		_offset = offset;
		if (_collider != nullptr)
		{
			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(_offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
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
			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(_offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
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
			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(_offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
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
			Vector2 scale = GetScale();
			_collider->SetAsCapsuleShape(_offset * scale, _height * scale.GetY(), _radius * scale.GetX(), _rotation);
		}
	}

	/// @brief 
	/// @return 
	float CapsuleCollider2dComponent::GetRotation() const
	{
		return _rotation;
	}
}
