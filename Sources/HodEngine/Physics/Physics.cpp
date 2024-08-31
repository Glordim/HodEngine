#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/Physics/Body.hpp"

namespace hod::physics
{
	/// @brief 
	/// @param  
	_SingletonConstructor(Physics)
	{

	}

	/// @brief 
	Physics::~Physics()
	{
		Clear();
	}

	/// @brief 
	void Physics::Clear()
	{
		for (Body* body : _bodies)
		{
			delete body;
		}
		_bodies.clear();
	}

	/// @brief 
	/// @return 
	DebugDrawer* Physics::GetDebugDrawer()
	{
		return _debugDrawer;
	}
}
