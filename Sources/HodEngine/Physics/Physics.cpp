#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Physics.hpp"
#include "HodEngine/Physics/World.hpp"

#include "HodEngine/Physics/Box2d/Box2dPhysics.hpp"

namespace hod::inline physics
{
	/// @brief
	/// @return
	Physics* Physics::CreatePhysicsInstance()
	{
		return Box2dPhysics::CreateInstance();
	}

	/// @brief
	void Physics::DestroyPhysicsInstance()
	{
		Box2dPhysics::DestroyInstance();
	}

	/// @brief
	/// @param
	_SingletonConstructor(Physics) {}

	/// @brief
	Physics::~Physics()
	{
		Clear();
	}

	/// @brief
	void Physics::Clear()
	{
		for (World* world : _worlds)
		{
			DefaultAllocator::GetInstance().Delete(world);
		}
		_worlds.Clear();
	}
}
