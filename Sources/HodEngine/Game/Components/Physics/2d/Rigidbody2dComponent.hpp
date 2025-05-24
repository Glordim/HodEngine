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

		physics::Body*	GetInternalBody() const;

		void			SetMode(Mode mode);
		Mode			GetMode() const;

		void			SetGravityScale(float gravityScale);
		float			GetGravityScale() const;

		void			SetVelocity(const Vector2& velocity);
		Vector2			GetVelocity() const;

		Event<const physics::Collision&>&		GetOnCollisionEnterEvent();
		Event<const physics::Collision&>&		GetOnCollisionExitEvent();

		Event<const physics::Collider&, const physics::Collider&>&		GetOnTriggerEnterEvent();
		Event<const physics::Collider&, const physics::Collider&>&		GetOnTriggerExitEvent();

	private:

		physics::Body*	_body = nullptr;
		
		Mode			_mode = Mode::Static;
		float			_gravityScale = 1.0f;

		Event<const physics::Collision&>	_onCollisionEnterEvent;
		Event<const physics::Collision&>	_onCollisionExitEvent;

		Event<const physics::Collider&, const physics::Collider&>	_onTriggerEnterEvent;
		Event<const physics::Collider&, const physics::Collider&>	_onTriggerExitEvent;
	};
}
