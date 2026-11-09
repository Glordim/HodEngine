#pragma once
#include "HodEngine/PhysicsComponents/Export.hpp"

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Game/Component.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::inline physics
{
	class Body;
	struct Collision;
	class Collider;
}

namespace hod::inline physics
{
	class Collider2dComponent;

	struct CollisionEvent
	{
		Collider2dComponent* _collider;
		Collider2dComponent* _other;
		Vector2              _normal;
	};

	struct TriggerEvent
	{
		Collider2dComponent* _collider;
		Collider2dComponent* _other;
	};

	/// @brief
	class HOD_PHYSICS_COMPONENTS_API Rigidbody2dComponent : public Component
	{
		REFLECTED_CLASS(Rigidbody2dComponent, Component)

	public:
		enum class Mode : uint8_t
		{
			Static = 0,
			Kinematic,
			Dynamic,

			Count
		};
		REFLECTED_ENUM(HOD_PHYSICS_COMPONENTS_API, Mode);

	public:
		Rigidbody2dComponent() = default;
		Rigidbody2dComponent(const Rigidbody2dComponent&) = delete;
		Rigidbody2dComponent(Rigidbody2dComponent&&) = delete;
		~Rigidbody2dComponent() override;

		void operator=(const Rigidbody2dComponent&) = delete;
		void operator=(Rigidbody2dComponent&&) = delete;

	public:
		void OnConstruct() override;
		void OnAwake() override;
		void OnStart() override;
		void OnEnable() override;
		void OnFixedUpdate() override;
		void OnDisable() override;
		void OnDestruct() override;

		/// @brief Places its body where its node is. Done each fixed step, and by the editor while the simulation doesn't run (editing, paused)
		/// for the physics shown to follow the edited nodes
		void SyncBodyWithNode();

		Body* GetInternalBody() const;
		Vector2        GetParentOffset(Collider2dComponent* collider) const;

		void SetMode(Mode mode);
		Mode GetMode() const;

		void  SetGravityScale(float gravityScale);
		float GetGravityScale() const;

		/// @brief Its rotation locked: the collisions, the torques don't turn it (see Body::SetFixedRotation)
		void SetFixedRotation(bool fixedRotation);
		bool GetFixedRotation() const;

		/// @brief For a fast dynamic body: it doesn't go through the other dynamic and kinematic bodies (see Body::SetContinuousCollision)
		void SetContinuousCollision(bool continuousCollision);
		bool GetContinuousCollision() const;

		/// @brief Its position locked on these world axes: it only moves along the other one (see Body::SetFreezePosition)
		void SetFreezePositionX(bool freeze);
		bool GetFreezePositionX() const;
		void SetFreezePositionY(bool freeze);
		bool GetFreezePositionY() const;

		void    SetVelocity(const Vector2& velocity);
		Vector2 GetVelocity() const;

		void AddForce(const Vector2& force);
		void AddImpulse(const Vector2& impulse);

		Event<const CollisionEvent&>& GetOnCollisionEnterEvent();
		Event<const CollisionEvent&>& GetOnCollisionExitEvent();

		Event<const TriggerEvent&>& GetOnTriggerEnterEvent();
		Event<const TriggerEvent&>& GetOnTriggerExitEvent();

	private:
		/// @return false without a node
		bool GetNodeTransform(Vector2& position, float& rotation) const;

	private:
		Body* _body = nullptr;

		Mode  _mode = Mode::Static;
		float _gravityScale = 1.0f;
		bool  _fixedRotation = false;
		bool  _continuousCollision = false;
		bool  _freezePositionX = false;
		bool  _freezePositionY = false;

		Event<const CollisionEvent&> _onCollisionEnterEvent;
		Event<const CollisionEvent&> _onCollisionExitEvent;

		Event<const TriggerEvent&> _onTriggerEnterEvent;
		Event<const TriggerEvent&> _onTriggerExitEvent;
	};
}
