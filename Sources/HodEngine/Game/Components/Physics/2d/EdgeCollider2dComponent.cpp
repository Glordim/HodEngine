#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/EdgeCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"

#include "HodEngine/Physics/Body.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(EdgeCollider2dComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &EdgeCollider2dComponent::_start, "Start", &EdgeCollider2dComponent::SetStart);
		AddPropertyT(reflectionDescriptor, &EdgeCollider2dComponent::_end, "End", &EdgeCollider2dComponent::SetEnd);
	}

	/// @brief 
	void EdgeCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Rigidbody2dComponent* rigidbody = GetRigidbody();
		Vector2 parentOffset = rigidbody->GetParentOffset(this);

		Vector2 scale = GetScale();
		_collider = rigidbody->GetInternalBody()->AddEdgeShape(_isTrigger, parentOffset + _start * scale, parentOffset + _end * scale);
	}

	/// @brief 
	/// @param start 
	void EdgeCollider2dComponent::SetStart(const Vector2& start)
	{
		_start = start;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsEdge(parentOffset + _start * scale, parentOffset + _end * scale);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& EdgeCollider2dComponent::GetStart() const
	{
		return _start;
	}

	/// @brief 
	/// @param end 
	void EdgeCollider2dComponent::SetEnd(const Vector2& end)
	{
		_end = end;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2 parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsEdge(parentOffset + _start * scale, parentOffset + _end * scale);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& EdgeCollider2dComponent::GetEnd() const
	{
		return _end;
	}
}
