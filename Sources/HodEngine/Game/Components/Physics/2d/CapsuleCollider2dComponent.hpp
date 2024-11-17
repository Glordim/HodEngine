#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"

#include <vector>

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API CapsuleCollider2dComponent : public Collider2dComponent
	{
		REFLECTED_CLASS(CapsuleCollider2dComponent, Collider2dComponent)

	public:

						CapsuleCollider2dComponent() = default;
						CapsuleCollider2dComponent(const CapsuleCollider2dComponent&) = delete;
						CapsuleCollider2dComponent(CapsuleCollider2dComponent&&) = delete;
						~CapsuleCollider2dComponent() override = default;

		void			operator=(const CapsuleCollider2dComponent&) = delete;
		void			operator=(CapsuleCollider2dComponent&&) = delete;

	public:

		void			OnAwake() override;

		void			SetOffset(const Vector2& offset);
		const Vector2&	GetOffset() const;
		
		void			SetHeight(float height);
		float			GetHeight() const;

		void			SetRadius(float radius);
		float			GetRadius() const;

		void			SetRotation(float rotation);
		float			GetRotation() const;

	private:

		Vector2			_offset = Vector2::Zero;
		float			_height = 1.0f;
		float			_radius = 0.20f;
		float			_rotation = 0.0f;
	};
}
