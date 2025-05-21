#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/World.hpp"

namespace hod::physics
{
	/// @brief 
	World::~World()
	{
		Clear();
	}

	/// @brief 
	/// @return 
	DebugDrawer* World::GetDebugDrawer()
	{
		return _debugDrawer;
	}

	/// @brief 
	void World::Clear()
	{
		for (Body* body : _bodies)
		{
			delete body;
		}
		_bodies.clear();
	}
}
