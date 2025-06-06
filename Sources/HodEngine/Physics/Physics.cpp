#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Physics.hpp"
#include "HodEngine/Physics/World.hpp"

#include "HodEngine/Physics/Box2d/PhysicsBox2d.hpp"

namespace hod::physics
{
	/// @brief 
	/// @return 
	Physics* Physics::CreatePhysicsInstance()
	{
		return PhysicsBox2d::CreateInstance();
	}

	/// @brief 
	void Physics::DestroyPhysicsInstance()
	{
		PhysicsBox2d::DestroyInstance();
	}

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
		for (World* world : _worlds)
		{
			DefaultAllocator::GetInstance().Delete(world);
		}
		_worlds.clear();
	}
}
