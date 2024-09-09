#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/BodyBox2d.hpp"
#include "HodEngine/Physics/Box2d/ColliderBox2d.hpp"

#include "Physics.hpp"

#include <box2d/box2d.h>

#include <HodEngine/Renderer/BoundingBox.hpp>

#include <algorithm>

#include "HodEngine/Core/Math/Math.hpp"

namespace hod::physics
{
	/// @brief 
	/// @param b2BodyId 
	BodyBox2d::BodyBox2d(b2BodyId b2BodyId)
	: _b2BodyId(b2BodyId)
	{
	}

	/// @brief 
	/// @param startPosition 
	/// @param endPosition 
	Collider* BodyBox2d::AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition)
	{
		ColliderBox2d* collider = new ColliderBox2d(this);
		collider->SetAsEdge(startPosition, endPosition);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param position 
	/// @param radius 
	Collider* BodyBox2d::AddCircleShape(const Vector2& position, float radius)
	{
		ColliderBox2d* collider = new ColliderBox2d(this);
		collider->SetAsCircleShape(position, radius);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param position 
	/// @param height 
	/// @param radius 
	/// @param angle 
	/// @return 
	Collider* BodyBox2d::AddCapsuleShape(const Vector2& position, float height, float radius, float angle)
	{
		ColliderBox2d* collider = new ColliderBox2d(this);
		collider->SetAsCapsuleShape(position, height, radius, angle);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param position 
	/// @param size 
	/// @param angle 
	/// @param density 
	Collider* BodyBox2d::AddBoxShape(const Vector2& position, const Vector2& size, float angle)
	{
		ColliderBox2d* collider = new ColliderBox2d(this);
		collider->SetAsBoxShape(position, size, angle);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param vertices 
	Collider* BodyBox2d::AddConvexShape(const std::vector<const Vector2>& vertices)
	{
		ColliderBox2d* collider = new ColliderBox2d(this);
		collider->SetAsConvexShape(vertices);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param position 
	/// @param rotation 
	/// @param scale 
	void BodyBox2d::SetTransform(const Vector2& position, float rotation, const Vector2& scale)
	{
		// todo scale
		b2Body_SetTransform(_b2BodyId, { position.GetX(), position.GetY() }, b2MakeRot(math::DegreeToRadian(rotation)));
	}

	/// @brief 
	/// @return 
	Vector2 BodyBox2d::GetPosition() const
	{
		b2Vec2 position = b2Body_GetPosition(_b2BodyId);
		return Vector2(position.x, position.y);
	}

	/// @brief 
	/// @return 
	float BodyBox2d::GetRotation() const
	{
		b2Rot rotation = b2Body_GetRotation(_b2BodyId);
		return b2Rot_GetAngle(rotation);
	}

	/// @brief 
	/// @return 
	Body::Type BodyBox2d::GetType() const
	{
		return static_cast<Type>(b2Body_GetType(_b2BodyId));
	}

	/// @brief 
	/// @param type 
	void BodyBox2d::SetType(Type type)
	{
		b2Body_SetType(_b2BodyId, static_cast<b2BodyType>(type));
		b2Body_SetAwake(_b2BodyId, true);
	}

	/// @brief 
	/// @return 
	b2BodyId BodyBox2d::GetB2Actor() const
	{
		return _b2BodyId;
	}
}
