#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Body.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"

#include <box2d/id.h>

namespace hod::physics
{
	class ColliderBox2d;

	/// @brief 
	class HOD_PHYSICS_API BodyBox2d : public Body
	{
	public:
							BodyBox2d(b2BodyId b2BodyId);
							~BodyBox2d() override = default;

	public:

		void				ClearAllShapes() override;
		Collider*			AddEdgeShape(bool isTrigger, const Vector2& startPosition, const Vector2& endPosition) override;
		Collider*			AddCircleShape(bool isTrigger, const Vector2& position, float radius) override;
		Collider*			AddCapsuleShape(bool isTrigger, const Vector2& position, float height, float radius, float angle) override;
		Collider*			AddBoxShape(bool isTrigger, const Vector2& position, const Vector2& size, float angle) override;
		Collider*			AddConvexShape(bool isTrigger, const Vector<const Vector2>& vertices) override;

		void				SetEnabled(bool enabled) override;
		void				SetTransform(const Vector2& position, float angle, const Vector2& scale) override;

		Vector2				GetPosition() const override;
		float				GetRotation() const override;

		void				GetCollisions(Vector<Collision>& collisions) override;

		Type				GetType() const override;
		void				SetType(Type type) override;

		float				GetGravityScale() const override;
		void				SetGravityScale(float gravityScale) override;

		void				SetVelocity(const Vector2& velocity) override;
		Vector2				GetVelocity() const override;

		void				AddForce(const Vector2& force) override;
		void				AddImpulse(const Vector2& impulse) override;

		b2BodyId			GetB2Actor() const;
		ColliderBox2d*		FindColliderByB2ShapeId(b2ShapeId shapeId) const;

	private:

		b2BodyId			_b2BodyId = b2_nullBodyId;
	};
}
