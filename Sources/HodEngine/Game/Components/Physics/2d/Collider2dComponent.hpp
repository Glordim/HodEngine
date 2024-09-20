#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"

#include <vector>

namespace hod::physics
{
	class Collider;
}

namespace hod::game
{
	class Rigidbody2dComponent;

		/*
	struct HOD_GAME_API EdgeShape : public Shape
	{
		REFLECTED_CLASS(EdgeShape, Shape, HOD_GAME_API)
	public:

		~EdgeShape() override = default;

		Vector2 _start;
		Vector2 _end;
	};

	struct HOD_GAME_API CircleShape : public Shape
	{
		REFLECTED_CLASS(CircleShape, Shape, HOD_GAME_API)
	public:

		~CircleShape() override = default;

		Vector2	_origin;
		float	_radius;
	};

	struct HOD_GAME_API BoxShape : public Shape
	{
		REFLECTED_CLASS(BoxShape, Shape, HOD_GAME_API)
	public:

		~BoxShape() override = default;

		Vector2	_origin;
		Vector2	_size;
		float	_angle;
	};
		*/

	/// @brief 
	class HOD_GAME_API Collider2dComponent : public Component
	{
		REFLECTED_CLASS(Collider2dComponent, Component, HOD_GAME_API)

	public:

						Collider2dComponent();
						Collider2dComponent(const Collider2dComponent&) = delete;
						Collider2dComponent(Collider2dComponent&&) = delete;
						~Collider2dComponent() override;

		void			operator=(const Collider2dComponent&) = delete;
		void			operator=(Collider2dComponent&&) = delete;

	public:

		void			OnConstruct() override;
		void			OnAwake() override;

		std::shared_ptr<Rigidbody2dComponent>	GetRigidbody() const;
		Vector2									GetScale() const;

	protected:

		physics::Collider*	_collider = nullptr;

	private:

		bool			_isTrigger = false;
		float			_friction = 0.5f;
		float			_density = 1.0f;

		std::weak_ptr<Rigidbody2dComponent>	_rigidbody;
	};
}
