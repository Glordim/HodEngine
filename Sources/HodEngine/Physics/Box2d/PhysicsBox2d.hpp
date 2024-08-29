#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Physics.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include <vector>
#include <cstdint>

#include <box2d/id.h>

namespace hod
{
	class Vector2;
}

namespace hod::physics
{
	class BodyBox2d;
	class DebugDrawer;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct HOD_PHYSICS_API RaycastResult
	{
		Body* _bodyCollided = nullptr;
	};

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_PHYSICS_API PhysicsBox2d : public Physics
	{
		_SingletonOverride(PhysicsBox2d)

	public:

		enum DebugDrawFlag
		{
			Shape = 0,
			Join,
			AABB,
			Pair,
			CenterOfMass
		};

	protected:

							~PhysicsBox2d();

	public:

		bool				Init();
		void				Clear();

		Body*				CreateBody();
		void				DeleteBody(Body* body);

		void				Update(float dt);

		bool				Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result);

		//void				SetDebugDrawer(b2Draw* debugDrawer);
		//void				SetDebugDrawFlags(DebugDrawFlag flag, bool enabled);

	private:

		b2WorldId			_worldId = {};

		int32_t				_velocityIterations = 8;
		int32_t				_positionIterations = 3;

		std::vector<Body*>	_bodies;
	};
}
