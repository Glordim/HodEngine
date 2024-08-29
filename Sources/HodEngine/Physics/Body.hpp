#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <vector>
#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::physics
{
	/// @brief 
	class HOD_PHYSICS_API Body
	{
	public:

		enum Type
		{
			Static,
			Kinematic,
			Dynamic,
		};

	public:
							Body() = default;
		virtual				~Body() = default;

	public:

		virtual void		AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition) = 0;
		virtual void		AddCircleShape(const Vector2& position, float radius) = 0;
		virtual void		AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density) = 0;
		virtual void		AddConvexShape(const std::vector<const Vector2>& vertices) = 0;

		virtual void		SetTransform(const Vector2& position, float angle, const Vector2& scale) = 0;

		virtual Vector2		GetPosition() const = 0;
		virtual float		GetRotation() const = 0;

		virtual Type		GetType() const = 0;
		virtual void		SetType(Type type) = 0;
	};
}
