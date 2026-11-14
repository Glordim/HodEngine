#include "HodEngine/PhysicsComponents/Pch.hpp"
#include "HodEngine/PhysicsComponents/Components/MultiShapeCollider2dComponent.hpp"
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
	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent::CircleShape, reflectionDescriptor)
	{
		HOD_PROPERTY(_origin);
		HOD_PROPERTY(_radius);
	}

	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent::BoxShape, reflectionDescriptor)
	{
		HOD_PROPERTY(_origin);
		HOD_PROPERTY(_size);
		HOD_PROPERTY(_angle);
	}

	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_SHAPE_PLUS_OUTLINE);
		reflectionDescriptor.AddTrait<ReflectionTraitCategory>("Physics/Colliders");
		reflectionDescriptor.AddTrait<ReflectionTraitDescription>("2D collision shape made of several circles and boxes");
		HOD_PROPERTY(_circles);
		HOD_PROPERTY(_boxes);
	}

	/// @brief
	void MultiShapeCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody == nullptr)
		{
			return;
		}
		Vector2               parentOffset = rigidbody->GetParentOffset(this);

		Vector2        scale = GetScale();
		Body* body = rigidbody->GetInternalBody();

		for (const CircleShape& circle : _circles)
		{
			Collider* collider = body->AddCircleShape(_isTrigger, parentOffset + circle._origin * scale, circle._radius * scale.GetX());
			collider->SetUserData(this);
		}

		for (const BoxShape& box : _boxes)
		{
			Collider* collider = body->AddBoxShape(_isTrigger, parentOffset + box._origin * scale, box._size * scale, box._angle);
			collider->SetUserData(this);
		}
	}

	/// @brief
	void MultiShapeCollider2dComponent::ClearAllShapes()
	{
		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody != nullptr)
		{
			Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				body->ClearAllShapes();

				_circles.Clear();
				_boxes.Clear();
			}
		}
	}

	/// @brief
	/// @param circleShape
	void MultiShapeCollider2dComponent::AddCircleShape(const CircleShape& circleShape)
	{
		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody != nullptr)
		{
			Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				Rigidbody2dComponent* rigidbody = GetRigidbody();
				Vector2               parentOffset = rigidbody->GetParentOffset(this);

				Vector2            scale = GetScale();
				Collider* collider = body->AddCircleShape(_isTrigger, parentOffset + circleShape._origin * scale, circleShape._radius * scale.GetX());
				collider->SetUserData(this);
				_circles.push_back(circleShape);
			}
		}
	}

	/// @brief
	/// @return
	const Vector<MultiShapeCollider2dComponent::CircleShape>& MultiShapeCollider2dComponent::GetCircleShapes() const
	{
		return _circles;
	}

	/// @brief
	/// @param boxshape
	void MultiShapeCollider2dComponent::AddBoxShape(const BoxShape& boxshape)
	{
		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody != nullptr)
		{
			Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				Rigidbody2dComponent* rigidbody = GetRigidbody();
				Vector2               parentOffset = rigidbody->GetParentOffset(this);

				Vector2            scale = GetScale();
				Collider* collider = body->AddBoxShape(_isTrigger, parentOffset + boxshape._origin * scale, boxshape._size * scale, boxshape._angle);
				collider->SetUserData(this);
				_boxes.push_back(boxshape);
			}
		}
	}

	/// @brief
	/// @return
	const Vector<MultiShapeCollider2dComponent::BoxShape>& MultiShapeCollider2dComponent::GetBoxShapes() const
	{
		return _boxes;
	}
}
