#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Collider.hpp"

#include <vector>
#include "HodEngine/Core/Math/Vector2.hpp"

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

		void	SetAsEdge(const Vector2& startPosition, const Vector2& endPosition) override;
		void	SetAsCircleShape(const Vector2& position, float radius) override;
		void	SetAsCapsuleShape(const Vector2& position, float height, float radius, float angle) override;
		void	SetAsBoxShape(const Vector2& position, const Vector2& size, float angle) override;
		void	SetAsConvexShape(const std::vector<const Vector2>& vertices) override;

		void	SetBounciness(float bounciness) override;

	private:

		b2ShapeId	_shape = b2_nullShapeId;
	};
}
