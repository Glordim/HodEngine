#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/Box2dPhysics.hpp"
#include "HodEngine/Physics/Box2d/Box2dWorld.hpp"
#include "HodEngine/Physics/Box2d/Box2dDebugDrawer.hpp"
#include "HodEngine/Math/Math.hpp"

#include <box2d/box2d.h>

#include <algorithm>

namespace hod::inline physics
{
	/// @brief 
	/// @param  
	_SingletonOverrideConstructor(Box2dPhysics)
	{
	}

	/// @brief 
	Box2dPhysics::~Box2dPhysics()
	{
		Clear();
	}

	/// @brief 
	/// @return 
	bool Box2dPhysics::Init()
	{
		return true;
	}

	World* Box2dPhysics::CreateWorld()
	{
		return DefaultAllocator::GetInstance().New<Box2dWorld>();
	}
}
