#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include "HodEngine/Core/Vector.hpp"

namespace hod::inline physics
{
	class World;
	
	/// @brief What receives the debug view of a physics World (see DebugDrawer::Draw), in world space: implemented by whoever draws it
	class HOD_PHYSICS_API DebugDrawTarget
	{
	public:
		virtual ~DebugDrawTarget() = default;

		/// @brief Convex, counter-clockwise
		virtual void Polygon(const Vector2* points, uint32_t pointCount, const Color& color, bool filled) = 0;
		virtual void Circle(const Vector2& center, float radius, const Color& color, bool filled) = 0;
		/// @brief start, end: the centers of its two half circles
		virtual void Capsule(const Vector2& start, const Vector2& end, float radius, const Color& color, bool filled) = 0;
		virtual void Segment(const Vector2& start, const Vector2& end, const Color& color) = 0;
		/// @brief pixelSize: in pixels, whatever the zoom
		virtual void Point(const Vector2& position, float pixelSize, const Color& color) = 0;
	};

	/// @brief The debug view of a physics World: its shapes, joints... (see the flags), given to a DebugDrawTarget
	class HOD_PHYSICS_API DebugDrawer
	{
	public:

		struct Flag
		{
			Flag(const char* label, uint32_t value)
			: _label(label)
			, _value(value)
			{

			}

			const char* _label = nullptr;
			uint32_t _value = 0;
		};

	public:
										DebugDrawer() = default;
		virtual							~DebugDrawer() = default;

	public:

		/// @brief The debug view of world, as it is now, into target
		virtual void						Draw(World* world, DebugDrawTarget& target) = 0;

		virtual const Vector<Flag>&	GetAvailableFlags() const = 0;
		virtual void						SetFlags(uint32_t flags) = 0;
		virtual uint32_t					GetFlags() const = 0;
	};
}
