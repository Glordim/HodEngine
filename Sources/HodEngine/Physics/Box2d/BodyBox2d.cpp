#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/BodyBox2d.hpp"
#include "HodEngine/Physics/Box2d/ColliderBox2d.hpp"
#include "HodEngine/Physics/Box2d/PhysicsBox2d.hpp"
#include "HodEngine/Physics/Collision.hpp"

#include "Physics.hpp"

#include <box2d/box2d.h>

#include <HodEngine/Renderer/BoundingBox.hpp>

#include <algorithm>
#include <cstdlib>

#include "HodEngine/Core/Math/Math.hpp"

namespace hod::physics
{
	/// @brief 
	/// @param b2BodyId 
	BodyBox2d::BodyBox2d(b2BodyId b2BodyId)
	: _b2BodyId(b2BodyId)
	{
		b2Body_SetUserData(_b2BodyId, this);
	}

	/// @brief 
	void BodyBox2d::ClearAllShapes()
	{
		for (Collider* collider : _colliders)
		{
			DefaultAllocator::GetInstance().Delete(collider);
		}
		_colliders.clear();
	}

	/// @brief 
	/// @param startPosition 
	/// @param endPosition 
	Collider* BodyBox2d::AddEdgeShape(bool isTrigger, const Vector2& startPosition, const Vector2& endPosition)
	{
		ColliderBox2d* collider = DefaultAllocator::GetInstance().New<ColliderBox2d>(this, isTrigger);
		collider->SetAsEdge(startPosition, endPosition);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param position 
	/// @param radius 
	Collider* BodyBox2d::AddCircleShape(bool isTrigger, const Vector2& position, float radius)
	{
		ColliderBox2d* collider = DefaultAllocator::GetInstance().New<ColliderBox2d>(this, isTrigger);
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
	Collider* BodyBox2d::AddCapsuleShape(bool isTrigger, const Vector2& position, float height, float radius, float angle)
	{
		ColliderBox2d* collider = DefaultAllocator::GetInstance().New<ColliderBox2d>(this, isTrigger);
		collider->SetAsCapsuleShape(position, height, radius, angle);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param position 
	/// @param size 
	/// @param angle 
	/// @param density 
	Collider* BodyBox2d::AddBoxShape(bool isTrigger, const Vector2& position, const Vector2& size, float angle)
	{
		ColliderBox2d* collider = DefaultAllocator::GetInstance().New<ColliderBox2d>(this, isTrigger);
		collider->SetAsBoxShape(position, size, angle);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param vertices 
	Collider* BodyBox2d::AddConvexShape(bool isTrigger, const Vector<const Vector2>& vertices)
	{
		ColliderBox2d* collider = DefaultAllocator::GetInstance().New<ColliderBox2d>(this, isTrigger);
		collider->SetAsConvexShape(vertices);
		_colliders.push_back(collider);
		return collider;
	}

	/// @brief 
	/// @param enabled 
	void BodyBox2d::SetEnabled(bool enabled)
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
	float BodyBox2d::GetGravityScale() const
	{
		return b2Body_GetGravityScale(_b2BodyId);
	}

	/// @brief 
	/// @param gravityScale 
	void BodyBox2d::SetGravityScale(float gravityScale)
	{
		b2Body_SetGravityScale(_b2BodyId, gravityScale);
	}

	/// @brief 
	/// @param velocity 
	void BodyBox2d::SetVelocity(const Vector2& velocity)
	{
		b2Body_SetLinearVelocity(_b2BodyId, { velocity.GetX(), velocity.GetY() });
	}

	/// @brief 
	/// @return 
	Vector2 BodyBox2d::GetVelocity() const
	{
		b2Vec2 velocity = b2Body_GetLinearVelocity(_b2BodyId);
		return Vector2(velocity.x, velocity.y);
	}

	/// @brief 
	/// @param force 
	void BodyBox2d::AddForce(const Vector2& force)
	{
		b2Body_ApplyForceToCenter(_b2BodyId, { force.GetX(), force.GetY() }, true);
	}

	/// @brief 
	/// @param force 
	void BodyBox2d::AddImpulse(const Vector2& impulse)
	{
		b2Body_ApplyLinearImpulseToCenter(_b2BodyId, { impulse.GetX(), impulse.GetY() }, true);
	}

	/// @brief 
	/// @return 
	b2BodyId BodyBox2d::GetB2Actor() const
	{
		return _b2BodyId;
	}

	/// @brief 
	/// @param shapeId 
	/// @return 
	ColliderBox2d* BodyBox2d::FindColliderByB2ShapeId(b2ShapeId shapeId) const
	{
		for (uint32_t index = 0; index < _colliders.size(); ++index)
		{
			ColliderBox2d* collider = (ColliderBox2d*)_colliders[index];
			if (collider != nullptr)
			{
				return collider;
			}
		}
		return nullptr;
	}

	/// @brief 
	/// @param collision 
	void BodyBox2d::GetCollisions(Vector<Collision>& collisions)
	{
		int bodyContactCapacity = b2Body_GetContactCapacity(_b2BodyId);
		b2ContactData* contactDatas = (b2ContactData*)alloca(bodyContactCapacity * sizeof(b2ContactData));
		int bodyContactCount = b2Body_GetContactData(_b2BodyId, contactDatas, bodyContactCapacity);

		//collisions.resize(bodyContactCount);
		for (int index = 0; index < bodyContactCount; ++index)
		{
			/*
			const b2ContactData& contactData = contactDatas[index];
			Collision& collision = collisions[index];
			collision._colliderA = PhysicsBox2d::GetInstance()->FindColliderByB2ShapeId(contactData.shapeIdA);
			collision._colliderB = PhysicsBox2d::GetInstance()->FindColliderByB2ShapeId(contactData.shapeIdB);
			collision._normal = Vector2(contactData.manifold.normal.x, contactData.manifold.normal.y);
			*/
		}
	}
}
