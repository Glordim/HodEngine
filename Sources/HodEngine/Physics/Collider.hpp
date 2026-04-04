#pragma once
#include "HodEngine/Physics/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Math/Vector2.hpp"

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

		void				SetUserData(void* userData);
		void*				GetUserData() const;

		virtual void		SetAsEdge(const math::Vector2& startPosition, const math::Vector2& endPosition) = 0;
		virtual void		SetAsCircleShape(const math::Vector2& position, float radius) = 0;
		virtual void		SetAsCapsuleShape(const math::Vector2& position, float height, float radius, float angle) = 0;
		virtual void		SetAsBoxShape(const math::Vector2& position, const math::Vector2& size, float angle) = 0;
		virtual void		SetAsConvexShape(const Vector<math::Vector2>& vertices) = 0;

		virtual void		SetBounciness(float bounciness) = 0;

		Body*				GetBody() const;

	private:

		Body*				_body = nullptr;
		void*				_userData = nullptr;
	};
}
