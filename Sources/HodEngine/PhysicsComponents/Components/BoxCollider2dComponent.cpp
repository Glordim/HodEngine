#include "HodEngine/PhysicsComponents/Pch.hpp"
#include "HodEngine/PhysicsComponents/Components/BoxCollider2dComponent.hpp"
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
	DESCRIBE_REFLECTED_CLASS(BoxCollider2dComponent, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_SQUARE_OUTLINE);
		reflectionDescriptor.AddTrait<ReflectionTraitCategory>("Physics/Colliders");
		reflectionDescriptor.AddTrait<ReflectionTraitDescription>("Rectangular 2D collision shape");
		AddPropertyT(reflectionDescriptor, &BoxCollider2dComponent::_offset, "Offset", &BoxCollider2dComponent::SetOffset);
		AddPropertyT(reflectionDescriptor, &BoxCollider2dComponent::_size, "Size", &BoxCollider2dComponent::SetSize);
		AddPropertyT(reflectionDescriptor, &BoxCollider2dComponent::_rotation, "Rotation", &BoxCollider2dComponent::SetRotation);
	}

	/// @brief
	void BoxCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody == nullptr)
		{
			return;
		}
		Vector2               parentOffset = rigidbody->GetParentOffset(this);

		Vector2 scale = GetScale();
		_collider = rigidbody->GetInternalBody()->AddBoxShape(_isTrigger, parentOffset + _offset * scale, _size * scale, _rotation);
		_collider->SetUserData(this);
	}

	/// @brief
	void BoxCollider2dComponent::SetOffset(const Vector2& offset)
	{
		_offset = offset;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2               parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsBoxShape(parentOffset + _offset * scale, _size * scale, _rotation);
		}
	}

	/// @brief
	/// @return
	const Vector2& BoxCollider2dComponent::GetOffset() const
	{
		return _offset;
	}

	/// @brief
	void BoxCollider2dComponent::SetSize(const Vector2& Size)
	{
		_size = Size;
		if (_collider != nullptr)
		{
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2               parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsBoxShape(parentOffset + _offset * scale, _size * scale, _rotation);
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
			Rigidbody2dComponent* rigidbody = GetRigidbody();
			Vector2               parentOffset = rigidbody->GetParentOffset(this);

			Vector2 scale = GetScale();
			_collider->SetAsBoxShape(parentOffset + _offset * scale, _size * scale, _rotation);
		}
	}

	/// @brief
	/// @return
	float BoxCollider2dComponent::GetRotation() const
	{
		return _rotation;
	}
}
