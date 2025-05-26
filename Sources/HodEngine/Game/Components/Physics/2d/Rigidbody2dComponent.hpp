#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Event.hpp"

#include <vector>

namespace hod::physics
{
	class Body;
	struct Collision;
	class Collider;
}

namespace hod::game
{
	class Collider2dComponent;
	
	struct CollisionEvent
	{
		Collider2dComponent*	_collider;
		Collider2dComponent*	_other;
		Vector2					_normal;
	};

	struct TriggerEvent
	{
		Collider2dComponent*	_collider;
		Collider2dComponent*	_other;
	};

	/// @brief 
	class HOD_GAME_API Rigidbody2dComponent : public Component
	{
		REFLECTED_CLASS(Rigidbody2dComponent, Component)

	public:

		enum class Mode : uint8_t
		{
			Static = 0,
			Kinematic,
			Dynamic,
		};


	public:

						Rigidbody2dComponent() = default;
						Rigidbody2dComponent(const Rigidbody2dComponent&) = delete;
						Rigidbody2dComponent(Rigidbody2dComponent&&) = delete;
						~Rigidbody2dComponent() override;

		void			operator=(const Rigidbody2dComponent&) = delete;
		void			operator=(Rigidbody2dComponent&&) = delete;

	public:

		void			OnConstruct() override;
		void			OnAwake() override;
		void			OnStart() override;
		void			OnEnable() override;
		void			OnFixedUpdate() override;
		void			OnDisable() override;
		void			OnDestruct() override;

		physics::Body*	GetInternalBody() const;
		Vector2			GetParentOffset(Collider2dComponent* collider) const;

		void			SetMode(Mode mode);
		Mode			GetMode() const;

		void			SetGravityScale(float gravityScale);
		float			GetGravityScale() const;

		void			SetVelocity(const Vector2& velocity);
		Vector2			GetVelocity() const;

		void			AddForce(const Vector2& force);
		void			AddImpulse(const Vector2& impulse);

		Event<const CollisionEvent&>&		GetOnCollisionEnterEvent();
		Event<const CollisionEvent&>&		GetOnCollisionExitEvent();

		Event<const TriggerEvent&>&		GetOnTriggerEnterEvent();
		Event<const TriggerEvent&>&		GetOnTriggerExitEvent();

	private:

		physics::Body*	_body = nullptr;
		
		Mode			_mode = Mode::Static;
		float			_gravityScale = 1.0f;

		Event<const CollisionEvent&>	_onCollisionEnterEvent;
		Event<const CollisionEvent&>	_onCollisionExitEvent;

		Event<const TriggerEvent&>	_onTriggerEnterEvent;
		Event<const TriggerEvent&>	_onTriggerExitEvent;
	};
}
