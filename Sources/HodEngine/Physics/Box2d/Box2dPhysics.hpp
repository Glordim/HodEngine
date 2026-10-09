#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Physics.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <cstdint>

#include <box2d/id.h>


namespace hod::inline physics
{
	class Box2dBody;
	class DebugDrawer;
	class Box2dCollider;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_PHYSICS_API Box2dPhysics : public Physics
	{
		_SingletonOverride(Box2dPhysics)

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

							~Box2dPhysics();

	public:

		bool				Init() override;

		World*				CreateWorld() override;
	};
}
