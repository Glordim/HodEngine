#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <vector>
#include "HodEngine/Physics/Body.hpp"

namespace hod
{
	class Vector2;
}

namespace hod::physics
{
	class DebugDrawer;

	/// @brief 
	struct HOD_PHYSICS_API RaycastResult
	{
		Body* _bodyCollided = nullptr;
	};

	/// @brief 
	class HOD_PHYSICS_API World
	{
	public:
							World() = default;
		virtual				~World();

	public:

		void				Clear();

		virtual Body*		CreateBody(Body::Type type, const Vector2& position, float rotation) = 0;
		virtual void		DeleteBody(Body* body) = 0;

		virtual void		Update(float dt) = 0;

		virtual bool		Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result) = 0;

		DebugDrawer*		GetDebugDrawer();

	protected:

		std::vector<Body*>	_bodies;

		DebugDrawer*		_debugDrawer = nullptr;
	};
}
