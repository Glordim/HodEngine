#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"

#include <vector>

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API EdgeCollider2dComponent : public Collider2dComponent
	{
		REFLECTED_CLASS(EdgeCollider2dComponent, Collider2dComponent, HOD_GAME_API)

	public:

						EdgeCollider2dComponent() = default;
						EdgeCollider2dComponent(const EdgeCollider2dComponent&) = delete;
						EdgeCollider2dComponent(EdgeCollider2dComponent&&) = delete;
						~EdgeCollider2dComponent() override = default;

		void			operator=(const EdgeCollider2dComponent&) = delete;
		void			operator=(EdgeCollider2dComponent&&) = delete;

	public:

		void			OnAwake() override;

		void			SetStart(const Vector2& start);
		const Vector2&	GetStart() const;

		void			SetEnd(const Vector2& end);
		const Vector2&	GetEnd() const;

	private:

		Vector2			_start = Vector2(-0.5f, 0.0f);
		Vector2			_end = Vector2(0.5f, 0.0f);
	};
}