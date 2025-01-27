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
		AddPropertyT(&reflectionDescriptor, &CircleShape::_origin, "Origin");
		AddPropertyT(&reflectionDescriptor, &CircleShape::_radius, "Radius");
	}

	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent::BoxShape, reflectionDescriptor)
	{
		AddPropertyT(&reflectionDescriptor, &BoxShape::_origin, "Origin");
		AddPropertyT(&reflectionDescriptor, &BoxShape::_size, "Size");
		AddPropertyT(&reflectionDescriptor, &BoxShape::_angle, "Angle");
	}

	DESCRIBE_REFLECTED_CLASS(MultiShapeCollider2dComponent, reflectionDescriptor)
	{
		AddPropertyT(&reflectionDescriptor, &MultiShapeCollider2dComponent::_circles, "Circles");
		AddPropertyT(&reflectionDescriptor, &MultiShapeCollider2dComponent::_boxes, "Boxes");
	}

	/// @brief 
	void MultiShapeCollider2dComponent::OnStart()
	{
		Collider2dComponent::OnStart();

		Vector2 scale = GetScale();
		physics::Body* body = GetRigidbody()->GetInternalBody();

		for (const CircleShape& circle : _circles)
		{
			body->AddCircleShape(_isTrigger, circle._origin * scale, circle._radius * scale.GetX());
		}

		for (const BoxShape& box : _boxes)
		{
			body->AddBoxShape(_isTrigger, box._origin * scale, box._size * scale, box._angle);
		}
	}

	/// @brief 
	/// @param circleShape 
	void MultiShapeCollider2dComponent::AddCircleShape(const CircleShape& circleShape)
	{
		std::shared_ptr<Rigidbody2dComponent> rigidbody = GetRigidbody();
		if (rigidbody != nullptr)
		{
			physics::Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				Vector2 scale = GetScale();
				body->AddCircleShape(_isTrigger, circleShape._origin * scale, circleShape._radius * scale.GetX());
				_circles.push_back(circleShape);
			}
		}
	}

	/// @brief 
	/// @param boxshape 
	void MultiShapeCollider2dComponent::AddBoxShape(const BoxShape& boxshape)
	{
		std::shared_ptr<Rigidbody2dComponent> rigidbody = GetRigidbody();
		if (rigidbody != nullptr)
		{
			physics::Body* body = rigidbody->GetInternalBody();
			if (body != nullptr)
			{
				Vector2 scale = GetScale();
				body->AddBoxShape(_isTrigger, boxshape._origin * scale, boxshape._size * scale, boxshape._angle);
				_boxes.push_back(boxshape);
			}
		}
	}
}
