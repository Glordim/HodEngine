#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"

#include <vector>

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API CircleCollider2dComponent : public Collider2dComponent
	{
		REFLECTED_CLASS(CircleCollider2dComponent, Collider2dComponent, HOD_GAME_API)

	public:

						CircleCollider2dComponent() = default;
						CircleCollider2dComponent(const CircleCollider2dComponent&) = delete;
						CircleCollider2dComponent(CircleCollider2dComponent&&) = delete;
						~CircleCollider2dComponent() override = default;

		void			operator=(const CircleCollider2dComponent&) = delete;
		void			operator=(CircleCollider2dComponent&&) = delete;

	public:

		void			OnAwake() override;

		void			SetOffset(const Vector2& offset);
		const Vector2&	GetOffset() const;

		void			SetRadius(float radius);
		float			GetRadius() const;

	private:

		Vector2			_offset = Vector2::Zero;
		float			_radius = 0.5f;
	};
}