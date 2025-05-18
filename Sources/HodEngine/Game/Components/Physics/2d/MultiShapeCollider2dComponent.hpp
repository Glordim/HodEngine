#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

#include <vector>

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API MultiShapeCollider2dComponent : public Collider2dComponent
	{
		REFLECTED_CLASS(MultiShapeCollider2dComponent, Collider2dComponent)

	public:

		struct CircleShape
		{
			REFLECTED_CLASS_NO_VIRTUAL(CircleShape)

			Vector2	_origin = Vector2::Zero;
			float	_radius = 1.0f;
		};

		struct BoxShape
		{
			REFLECTED_CLASS_NO_VIRTUAL(BoxShape)

			Vector2	_origin = Vector2::Zero;
			Vector2	_size = Vector2::One;
			float	_angle = 0.0f;
		};

	public:

						MultiShapeCollider2dComponent() = default;
						MultiShapeCollider2dComponent(const MultiShapeCollider2dComponent&) = delete;
						MultiShapeCollider2dComponent(MultiShapeCollider2dComponent&&) = delete;
						~MultiShapeCollider2dComponent() override = default;

		void			operator=(const MultiShapeCollider2dComponent&) = delete;
		void			operator=(MultiShapeCollider2dComponent&&) = delete;

	public:

		void			OnStart() override;

		void			ClearAllShapes();

		void			AddCircleShape(const CircleShape& circleShape);
		const std::vector<CircleShape>&	GetCircleShapes() const;

		void			AddBoxShape(const BoxShape& boxshape);
		const std::vector<BoxShape>&	GetBoxShapes() const;

	private:

		std::vector<CircleShape>	_circles;
		std::vector<BoxShape>		_boxes;
	};
}
