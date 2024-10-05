#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/BoxCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"

#include "HodEngine/Physics/Body.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(BoxCollider2dComponent, Collider2dComponent)
	{
		AddPropertyT(this, &BoxCollider2dComponent::_offset, "Offset", &BoxCollider2dComponent::SetOffset);
		AddPropertyT(this, &BoxCollider2dComponent::_size, "Size", &BoxCollider2dComponent::SetSize);
		AddPropertyT(this, &BoxCollider2dComponent::_rotation, "Rotation", &BoxCollider2dComponent::SetRotation);
	}

	/// @brief 
	void BoxCollider2dComponent::OnAwake()
	{
		Collider2dComponent::OnAwake();

		Vector2 scale = GetScale();
		_collider = GetRigidbody()->GetInternalBody()->AddBoxShape(_isTrigger, _offset * scale, _size * scale, _rotation);
	}

	/// @brief 
	void BoxCollider2dComponent::SetOffset(const Vector2& offset)
	{
		_offset = offset;
		if (_collider != nullptr)
		{
			Vector2 scale = GetScale();
			_collider->SetAsBoxShape(_offset * scale, _size * scale, _rotation);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& BoxCollider2dComponent::GetOffset() const
	{
		return _offset;
	}

	/// @brief 
	void BoxCollider2dComponent::SetSize(const Vector2& size)
	{
		_size = size;
		if (_collider != nullptr)
		{
			Vector2 scale = GetScale();
			_collider->SetAsBoxShape(_offset * scale, _size * scale, _rotation);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& BoxCollider2dComponent::GetSize() const
	{
		return _size;
	}

	/// @brief 
	void BoxCollider2dComponent::SetRotation(float rotation)
	{
		_rotation = rotation;
		if (_collider != nullptr)
		{
			Vector2 scale = GetScale();
			_collider->SetAsBoxShape(_offset * scale, _size * scale, _rotation);
		}
	}

	/// @brief 
	/// @return 
	float BoxCollider2dComponent::GetRotation() const
	{
		return _rotation;
	}
}
