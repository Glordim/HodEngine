#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"

#include "HodEngine/Physics/Body.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(CircleCollider2dComponent, Collider2dComponent)
	{
		AddPropertyT(this, &CircleCollider2dComponent::_offset, "Offset", &CircleCollider2dComponent::SetOffset);
		AddPropertyT(this, &CircleCollider2dComponent::_radius, "Radius", &CircleCollider2dComponent::SetRadius);
	}

	/// @brief 
	void CircleCollider2dComponent::OnAwake()
	{
		Collider2dComponent::OnAwake();

		Vector2 scale = GetScale();
		_collider = GetRigidbody()->GetInternalBody()->AddCircleShape(_offset * scale, _radius * std::max(scale.GetX(), scale.GetY()));
	}

	/// @brief 
	void CircleCollider2dComponent::SetOffset(const Vector2& offset)
	{
		_offset = offset;
		if (_collider != nullptr)
		{
			Vector2 scale = GetScale();
			_collider->SetAsCircleShape(_offset * scale, _radius * std::max(scale.GetX(), scale.GetY()));
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
			Vector2 scale = GetScale();
			_collider->SetAsCircleShape(_offset * scale, _radius * std::max(scale.GetX(), scale.GetY()));
		}
	}

	/// @brief 
	/// @return 
	float CircleCollider2dComponent::GetRadius() const
	{
		return _radius;
	}
}
