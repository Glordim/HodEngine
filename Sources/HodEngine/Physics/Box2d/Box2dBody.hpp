#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Body.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Math/Vector2.hpp"

#include <box2d/id.h>

namespace hod::inline physics
{
	class Box2dCollider;

	/// @brief 
	class HOD_PHYSICS_API Box2dBody : public Body
	{
	public:
							Box2dBody(b2BodyId b2BodyId);
							~Box2dBody() override;

	public:

		void				ClearAllShapes() override;
		Collider*			AddEdgeShape(bool isTrigger, const Vector2& startPosition, const Vector2& endPosition) override;
		Collider*			AddCircleShape(bool isTrigger, const Vector2& position, float radius) override;
		Collider*			AddCapsuleShape(bool isTrigger, const Vector2& position, float height, float radius, float angle) override;
		Collider*			AddBoxShape(bool isTrigger, const Vector2& position, const Vector2& size, float angle) override;
		Collider*			AddConvexShape(bool isTrigger, const Vector<Vector2>& vertices) override;

		void				SetEnabled(bool enabled) override;
		void				SetTransform(const Vector2& position, float angle, const Vector2& scale) override;

		Vector2				GetPosition() const override;
		float				GetRotation() const override;

		void				GetCollisions(Vector<Collision>& collisions) override;

		Type				GetType() const override;
		void				SetType(Type type) override;

		float				GetGravityScale() const override;
		void				SetGravityScale(float gravityScale) override;

		bool				IsFixedRotation() const override;
		void				SetFixedRotation(bool fixedRotation) override;

		void				SetFreezePosition(bool x, bool y) override;
		void				SetContinuousCollision(bool continuousCollision) override;
		void				MoveTo(const Vector2& position, float angle, float timeStep) override;

		void				SetVelocity(const Vector2& velocity) override;
		Vector2				GetVelocity() const override;

		void				AddForce(const Vector2& force) override;
		void				AddImpulse(const Vector2& impulse) override;

		b2BodyId			GetB2Actor() const;
		Box2dCollider*		FindColliderByB2ShapeId(b2ShapeId shapeId) const;

	private:

		void				DestroyPositionLock();

		/// @brief It and the bodies touching it (moved, they must react)
		void				WakeUpWithContacts();

	private:

		b2BodyId			_b2BodyId = b2_nullBodyId;

		// The frozen position (SetFreezePosition): a joint to a static body without shapes, moved along with the body (SetTransform). A wheel
		// joint without spring for one axis (a line, rotation free), a revolute joint for both (a point, rotation free)
		b2BodyId			_positionLockBodyId = b2_nullBodyId;
	};
}
