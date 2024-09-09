#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"

#include <vector>

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API BoxCollider2dComponent : public Collider2dComponent
	{
		REFLECTED_CLASS(BoxCollider2dComponent, Collider2dComponent, HOD_GAME_API)

	public:

						BoxCollider2dComponent() = default;
						BoxCollider2dComponent(const BoxCollider2dComponent&) = delete;
						BoxCollider2dComponent(BoxCollider2dComponent&&) = delete;
						~BoxCollider2dComponent() override = default;

		void			operator=(const BoxCollider2dComponent&) = delete;
		void			operator=(BoxCollider2dComponent&&) = delete;

	public:

		void			OnAwake() override;

		void			SetOffset(const Vector2& offset);
		const Vector2&	GetOffset() const;

		void			SetSize(const Vector2& size);
		const Vector2&	GetSize() const;

		void			SetRotation(float rotation);
		float			GetRotation() const;

	private:

		Vector2			_offset = Vector2::Zero;
		Vector2			_size = Vector2::One;
		float			_rotation = 0.0f;
	};
}
