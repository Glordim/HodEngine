#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API BoxCollider2dComponent : public Collider2dComponent
	{
		REFLECTED_CLASS(BoxCollider2dComponent, Collider2dComponent)

	public:

						BoxCollider2dComponent() = default;
						BoxCollider2dComponent(const BoxCollider2dComponent&) = delete;
						BoxCollider2dComponent(BoxCollider2dComponent&&) = delete;
						~BoxCollider2dComponent() override = default;

		void			operator=(const BoxCollider2dComponent&) = delete;
		void			operator=(BoxCollider2dComponent&&) = delete;

	public:

		void			OnStart() override;

		void			SetOffset(const math::Vector2& offset);
		const math::Vector2&	GetOffset() const;

		void			SetSize(const math::Vector2& size);
		const math::Vector2&	GetSize() const;

		void			SetRotation(float rotation);
		float			GetRotation() const;

	private:

		math::Vector2			_offset = math::Vector2::Zero;
		math::Vector2			_size = math::Vector2::One;
		float			_rotation = 0.0f;
	};
}
