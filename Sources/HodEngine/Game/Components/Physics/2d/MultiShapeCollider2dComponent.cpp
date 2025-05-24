#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/MultiShapeCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"

#include "HodEngine/Physics/Body.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent::CircleShape, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CircleShape::_origin, "Origin");
		AddPropertyT(reflectionDescriptor, &CircleShape::_radius, "Radius");
	}

	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent::BoxShape, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &BoxShape::_origin, "Origin");
		AddPropertyT(reflectionDescriptor, &BoxShape::_size, "Size");
		AddPropertyT(reflectionDescriptor, &BoxShape::_angle, "Angle");
	}

	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MultiShapeCollider2dComponent::_circles, "Circles");
		AddPropertyT(reflectionDescriptor, &MultiShapeCollider2dComponent::_boxes, "Boxes");
	}

	/// @brief 
	void MultiShapeCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Rigidbody2dComponent* rigidbody = GetRigidbody();
		Vector2 parentOffset = rigidbody->GetParentOffset(this);

		Vector2 scale = GetScale();
		physics::Body* body = rigidbody->GetInternalBody();

		for (const CircleShape& circle : _circles)
		{
			body->AddCircleShape(_isTrigger, parentOffset + circle._origin * scale, circle._radius * scale.GetX());
		}

		for (const BoxShape& box : _boxes)
		{
			body->AddBoxShape(_isTrigger, parentOffset + box._origin * scale, box._size * scale, box._angle);
		}
	}

	/// @brief 
	void MultiShapeCollider2dComponent::ClearAllShapes()
	{
		Rigidbody2dComponent* rigidbody = GetRigidbody();
		if (rigidbody != nullptr)
		{
			physics::Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				body->ClearAllShapes();

				_circles.clear();
				_boxes.clear();
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
			physics::Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				Rigidbody2dComponent* rigidbody = GetRigidbody();
				Vector2 parentOffset = rigidbody->GetParentOffset(this);

				Vector2 scale = GetScale();
				body->AddCircleShape(_isTrigger, parentOffset + circleShape._origin * scale, circleShape._radius * scale.GetX());
				_circles.push_back(circleShape);
			}
		}
	}

	/// @brief 
	/// @return 
	const std::vector<MultiShapeCollider2dComponent::CircleShape>& MultiShapeCollider2dComponent::GetCircleShapes() const
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
			physics::Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				Rigidbody2dComponent* rigidbody = GetRigidbody();
				Vector2 parentOffset = rigidbody->GetParentOffset(this);

				Vector2 scale = GetScale();
				body->AddBoxShape(_isTrigger, parentOffset + boxshape._origin * scale, boxshape._size * scale, boxshape._angle);
				_boxes.push_back(boxshape);
			}
		}
	}

	/// @brief 
	/// @return 
	const std::vector<MultiShapeCollider2dComponent::BoxShape>& MultiShapeCollider2dComponent::GetBoxShapes() const
	{
		return _boxes;
	}
}
