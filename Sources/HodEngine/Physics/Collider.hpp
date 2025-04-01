#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <vector>
#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::physics
{
	class Body;

	/// @brief 
	class HOD_PHYSICS_API Collider
	{
	public:
							Collider(Body* body);
		virtual				~Collider() = default;

	public:

		virtual void		SetAsEdge(const Vector2& startPosition, const Vector2& endPosition) = 0;
		virtual void		SetAsCircleShape(const Vector2& position, float radius) = 0;
		virtual void		SetAsCapsuleShape(const Vector2& position, float height, float radius, float angle) = 0;
		virtual void		SetAsBoxShape(const Vector2& position, const Vector2& size, float angle) = 0;
		virtual void		SetAsConvexShape(const std::vector<const Vector2>& vertices) = 0;

		virtual void		SetBounciness(float bounciness) = 0;

		Body*				GetBody() const;

	private:

		Body*				_body = nullptr;
	};
}
