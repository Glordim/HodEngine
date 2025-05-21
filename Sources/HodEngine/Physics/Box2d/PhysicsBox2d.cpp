#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/PhysicsBox2d.hpp"
#include "HodEngine/Physics/Box2d/WorldBox2d.hpp"
#include "HodEngine/Physics/Box2d/DebugDrawerBox2d.hpp"
#include "HodEngine/Core/Math/Math.hpp"

#include <box2d/box2d.h>

#include <algorithm>

namespace hod::physics
{
	/// @brief 
	/// @param  
	_SingletonOverrideConstructor(PhysicsBox2d)
	{
	}

	/// @brief 
	PhysicsBox2d::~PhysicsBox2d()
	{
		Clear();
	}

	/// @brief 
	/// @return 
	bool PhysicsBox2d::Init()
	{
		return true;
	}

	World* PhysicsBox2d::CreateWorld()
	{
		return new WorldBox2d();
	}
}
