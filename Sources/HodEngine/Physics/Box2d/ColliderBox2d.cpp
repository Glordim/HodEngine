#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/ColliderBox2d.hpp"
#include "HodEngine/Physics/Box2d/BodyBox2d.hpp"

#include <box2d/box2d.h>

namespace hod::physics
{
	/// @brief 
	/// @param body 
	ColliderBox2d::ColliderBox2d(Body* body)
	: Collider(body)
	{
		b2Segment segment;
		segment.point1 = { 0.0f, 0.0f };
		segment.point2 = { 1.0f, 0.0f };

		b2ShapeDef shapeDef = b2DefaultShapeDef();
		_shape = b2CreateSegmentShape(static_cast<BodyBox2d*>(body)->GetB2Actor(), &shapeDef, &segment);

		b2Shape_SetUserData(_shape, this);
	}

	/// @brief 
	ColliderBox2d::~ColliderBox2d()
	{
		b2BodyId body = b2Shape_GetBody(_shape);
		if (b2Body_IsValid(body))
		{
			b2DestroyShape(_shape);
		}
	}

	/// @brief 
	/// @param startPosition 
	/// @param endPosition 
	void ColliderBox2d::SetAsEdge(const Vector2& startPosition, const Vector2& endPosition)
	{
		b2Segment segment;
		segment.point1 = { startPosition.GetX(), startPosition.GetY() };
		segment.point2 = { endPosition.GetX(), endPosition.GetY() };

		b2Shape_SetSegment(_shape, &segment);
	}

	/// @brief 
	/// @param position 
	/// @param radius 
	void ColliderBox2d::SetAsCircleShape(const Vector2& position, float radius)
	{
		b2Circle circle;
		circle.center = { position.GetX(), position.GetY() };
		circle.radius = radius;

		b2Shape_SetCircle(_shape, &circle);
	}

	/// @brief 
	/// @param position 
	/// @param size 
	/// @param angle 
	/// @param density 
	void ColliderBox2d::SetAsBoxShape(const Vector2& position, const Vector2& size, float angle)
	{
		b2Polygon polygon = b2MakeOffsetBox(size.GetX(), size.GetY(), { position.GetX(), position.GetY() }, angle);
		b2Shape_SetPolygon(_shape, &polygon);
	}

	/// @brief 
	/// @param position 
	/// @param size 
	/// @param angle 
	/// @param density 
	void ColliderBox2d::SetAsCapsuleShape(const Vector2& position, float height, float radius, float angle)
	{
		b2Capsule capsule;
		capsule.center1 = { position.GetX(), position.GetY() + (height * 0.5f) - radius };
		capsule.center2 = { position.GetX(), position.GetY() - (height * 0.5f) + radius };
		capsule.radius = radius;
		b2Shape_SetCapsule(_shape, &capsule);
	}

	/// @brief 
	/// @param vertices 
	void ColliderBox2d::SetAsConvexShape(const std::vector<const Vector2>& vertices)
	{
		/*
		b2Polygon polygon = b2MakeBox(size.GetX(), size.GetY());
		polygon.centroid = { position.GetX(), position.GetY() };

		b2Shape_SetPolygon(_shape, &polygon);
		*/
	}
}