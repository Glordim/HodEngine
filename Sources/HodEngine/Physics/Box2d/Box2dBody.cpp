#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/Box2dBody.hpp"
#include "HodEngine/Physics/Box2d/Box2dCollider.hpp"
#include "HodEngine/Physics/Box2d/Box2dPhysics.hpp"
#include "HodEngine/Physics/Collision.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include <box2d/box2d.h>

#include <HodEngine/Renderer/BoundingBox.hpp>

#include <algorithm>
#include <cstdlib>

#include "HodEngine/Math/Math.hpp"

namespace hod::inline physics
{
	/// @brief
	/// @param b2BodyId
	Box2dBody::Box2dBody(b2BodyId b2BodyId)
	: _b2BodyId(b2BodyId)
	{
		b2Body_SetUserData(_b2BodyId, this);
	}

	/// @brief
	void Box2dBody::ClearAllShapes()
	{
		for (Collider* collider : _colliders)
		{
			DefaultAllocator::GetInstance().Delete(collider);
		}
		_colliders.Clear();
	}

	/// @brief
	/// @param startPosition
	/// @param endPosition
	Collider* Box2dBody::AddEdgeShape(bool isTrigger, const Vector2& startPosition, const Vector2& endPosition)
	{
		Box2dCollider* collider = DefaultAllocator::GetInstance().New<Box2dCollider>(this, isTrigger);
		collider->SetAsEdge(startPosition, endPosition);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief
	/// @param position
	/// @param radius
	Collider* Box2dBody::AddCircleShape(bool isTrigger, const Vector2& position, float radius)
	{
		Box2dCollider* collider = DefaultAllocator::GetInstance().New<Box2dCollider>(this, isTrigger);
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
	Collider* Box2dBody::AddCapsuleShape(bool isTrigger, const Vector2& position, float height, float radius, float angle)
	{
		Box2dCollider* collider = DefaultAllocator::GetInstance().New<Box2dCollider>(this, isTrigger);
		collider->SetAsCapsuleShape(position, height, radius, angle);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief
	/// @param position
	/// @param Size
	/// @param angle
	/// @param density
	Collider* Box2dBody::AddBoxShape(bool isTrigger, const Vector2& position, const Vector2& Size, float angle)
	{
		Box2dCollider* collider = DefaultAllocator::GetInstance().New<Box2dCollider>(this, isTrigger);
		collider->SetAsBoxShape(position, Size, angle);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief
	/// @param vertices
	Collider* Box2dBody::AddConvexShape(bool isTrigger, const Vector<Vector2>& vertices)
	{
		Box2dCollider* collider = DefaultAllocator::GetInstance().New<Box2dCollider>(this, isTrigger);
		collider->SetAsConvexShape(vertices);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief
	/// @param enabled
	void Box2dBody::SetEnabled(bool enabled)
	{
		if (enabled)
		{
			b2Body_Enable(_b2BodyId);
		}
		else
		{
			b2Body_Disable(_b2BodyId);
		}
	}

	/// @brief
	/// @param position
	/// @param rotation
	/// @param scale
	void Box2dBody::SetTransform(const Vector2& position, float rotation, const Vector2& scale)
	{
		// todo scale
		(void)scale;
		b2Body_SetTransform(_b2BodyId, {position.GetX(), position.GetY()}, b2MakeRot(DegreeToRadian(rotation)));
	}

	/// @brief
	/// @return
	Vector2 Box2dBody::GetPosition() const
	{
		b2Vec2 position = b2Body_GetPosition(_b2BodyId);
		return Vector2(position.x, position.y);
	}

	/// @brief
	/// @return
	float Box2dBody::GetRotation() const
	{
		b2Rot rotation = b2Body_GetRotation(_b2BodyId);
		return b2Rot_GetAngle(rotation);
	}

	/// @brief
	/// @return
	Body::Type Box2dBody::GetType() const
	{
		return static_cast<Type>(b2Body_GetType(_b2BodyId));
	}

	/// @brief
	/// @param type
	void Box2dBody::SetType(Type type)
	{
		b2Body_SetType(_b2BodyId, static_cast<b2BodyType>(type));
		b2Body_SetAwake(_b2BodyId, true);
	}

	/// @brief
	/// @return
	float Box2dBody::GetGravityScale() const
	{
		return b2Body_GetGravityScale(_b2BodyId);
	}

	/// @brief
	/// @param gravityScale
	void Box2dBody::SetGravityScale(float gravityScale)
	{
		b2Body_SetGravityScale(_b2BodyId, gravityScale);
	}

	/// @brief
	/// @param velocity
	void Box2dBody::SetVelocity(const Vector2& velocity)
	{
		b2Body_SetLinearVelocity(_b2BodyId, {velocity.GetX(), velocity.GetY()});
	}

	/// @brief
	/// @return
	Vector2 Box2dBody::GetVelocity() const
	{
		b2Vec2 velocity = b2Body_GetLinearVelocity(_b2BodyId);
		return Vector2(velocity.x, velocity.y);
	}

	/// @brief
	/// @param force
	void Box2dBody::AddForce(const Vector2& force)
	{
		b2Body_ApplyForceToCenter(_b2BodyId, {force.GetX(), force.GetY()}, true);
	}

	/// @brief
	/// @param force
	void Box2dBody::AddImpulse(const Vector2& impulse)
	{
		b2Body_ApplyLinearImpulseToCenter(_b2BodyId, {impulse.GetX(), impulse.GetY()}, true);
	}

	/// @brief
	/// @return
	b2BodyId Box2dBody::GetB2Actor() const
	{
		return _b2BodyId;
	}

	/// @brief
	/// @param shapeId
	/// @return
	Box2dCollider* Box2dBody::FindColliderByB2ShapeId(b2ShapeId shapeId) const
	{
		for (uint32_t index = 0; index < _colliders.Size(); ++index)
		{
			Box2dCollider* collider = (Box2dCollider*)_colliders[index];
			if (collider != nullptr && B2_ID_EQUALS(collider->GetShapeId(), shapeId))
			{
				return collider;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param collision
	void Box2dBody::GetCollisions(Vector<Collision>& collisions)
	{
		int            bodyContactCapacity = b2Body_GetContactCapacity(_b2BodyId);
		b2ContactData* contactDatas = (b2ContactData*)alloca(bodyContactCapacity * sizeof(b2ContactData));
		int            bodyContactCount = b2Body_GetContactData(_b2BodyId, contactDatas, bodyContactCapacity);

		(void)collisions; // TODO
		// collisions.Resize(bodyContactCount);
		for (int index = 0; index < bodyContactCount; ++index)
		{
			/*
			const b2ContactData& contactData = contactDatas[index];
			Collision& collision = collisions[index];
			collision._colliderA = Box2dPhysics::GetInstance()->FindColliderByB2ShapeId(contactData.shapeIdA);
			collision._colliderB = Box2dPhysics::GetInstance()->FindColliderByB2ShapeId(contactData.shapeIdB);
			collision._normal = Vector2(contactData.manifold.normal.x, contactData.manifold.normal.y);
			*/
		}
	}
}
