#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Math/Vector2.hpp"

#include <box2d/id.h>

namespace hod::physics
{
	class Body;

	/// @brief 
	class HOD_PHYSICS_API ColliderBox2d : public Collider
	{
	public:
				ColliderBox2d(Body* body, bool isTrigger);
				~ColliderBox2d() override;

	public:

		void	SetAsEdge(const math::Vector2& startPosition, const math::Vector2& endPosition) override;
		void	SetAsCircleShape(const math::Vector2& position, float radius) override;
		void	SetAsCapsuleShape(const math::Vector2& position, float height, float radius, float angle) override;
		void	SetAsBoxShape(const math::Vector2& position, const math::Vector2& size, float angle) override;
		void	SetAsConvexShape(const Vector<math::Vector2>& vertices) override;

		void	SetBounciness(float bounciness) override;

		b2ShapeId	GetShapeId() const;

	private:

		b2ShapeId	_shape = b2_nullShapeId;
	};
}
