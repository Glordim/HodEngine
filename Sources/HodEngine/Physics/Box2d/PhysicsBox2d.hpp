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
	class ColliderBox2d;

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

		World*				CreateWorld() override;

	private:

		b2WorldId			_worldId = {};
	};
}
