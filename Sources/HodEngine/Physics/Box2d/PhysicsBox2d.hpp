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

		bool				Init() override;

		Body*				CreateBody() override;
		void				DeleteBody(Body* body) override;

		void				Update(float dt) override;

		bool				Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result) override;

		//void				SetDebugDrawer(b2Draw* debugDrawer);
		//void				SetDebugDrawFlags(DebugDrawFlag flag, bool enabled);

	private:

		b2WorldId			_worldId = {};
	};
}
