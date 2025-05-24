#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Collision.hpp"

#include <vector>
#include <functional>
#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::physics
{
	class Collider;

	/// @brief 
	class HOD_PHYSICS_API Body
	{
	public:

		enum Type
		{
			Static,
			Kinematic,
			Dynamic,
		};

	public:
							Body() = default;
		virtual				~Body();

	public:

		void				SetMoveEventCallback(std::function<void(const Vector2& position, float rotation)> callback);
		const std::function<void(const Vector2& position, float rotation)>& GetMoveEventCallback() const;

		void				SetCollisionEnterCallback(std::function<void(const Collision& collision)> callback);
		const std::function<void(const Collision& collision)>& GetCollisionEnterCallback() const;

		void				SetCollisionExitCallback(std::function<void(const Collision& collision)> callback);
		const std::function<void(const Collision& collision)>& GetCollisionExitCallback() const;

		void				SetTriggerEnterCallback(std::function<void(const Collider& trigger, const Collider& visitor)> callback);
		const std::function<void(const Collider& trigger, const Collider& visitor)>& GetTriggerEnterCallback() const;

		void				SetTriggerExitCallback(std::function<void(const Collider& trigger, const Collider& visitor)> callback);
		const std::function<void(const Collider& trigger, const Collider& visitor)>& GetTriggerExitCallback() const;

		virtual void		ClearAllShapes() = 0;
		virtual Collider*	AddEdgeShape(bool isTrigger, const Vector2& startPosition, const Vector2& endPosition) = 0;
		virtual Collider*	AddCircleShape(bool isTrigger, const Vector2& position, float radius) = 0;
		virtual Collider*	AddCapsuleShape(bool isTrigger, const Vector2& position, float height, float radius, float angle) = 0;
		virtual Collider*	AddBoxShape(bool isTrigger, const Vector2& position, const Vector2& size, float angle) = 0;
		virtual Collider*	AddConvexShape(bool isTrigger, const std::vector<const Vector2>& vertices) = 0;

		virtual void		SetEnabled(bool enabled) = 0;
		virtual void		SetTransform(const Vector2& position, float angle, const Vector2& scale) = 0;

		virtual Vector2		GetPosition() const = 0;
		virtual float		GetRotation() const = 0;

		virtual void		SetVelocity(const Vector2& velocity) = 0;
		virtual Vector2		GetVelocity() const = 0;

		virtual void		GetCollisions(std::vector<Collision>& collisions) = 0;

		virtual Type		GetType() const = 0;
		virtual void		SetType(Type type) = 0;

		virtual float		GetGravityScale() const = 0;
		virtual void		SetGravityScale(float gravityScale) = 0;

	protected:

		std::vector<Collider*> _colliders;

		std::function<void(const Vector2& position, float rotation)> _moveCallback;
		std::function<void(const Collision& collision)> _collisionEnterCallback;
		std::function<void(const Collision& collision)> _collisionExitCallback;
		std::function<void(const Collider& trigger, const Collider& visitor)> _triggerEnterCallback;
		std::function<void(const Collider& trigger, const Collider& visitor)> _triggerExitCallback;
	};
}
