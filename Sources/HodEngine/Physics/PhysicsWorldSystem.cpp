#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/PhysicsWorldSystem.hpp"
#include "HodEngine/Physics/Physics.hpp"
#include "HodEngine/Physics/World.hpp"

#include <HodEngine/Game/World.hpp>

namespace hod::inline physics
{
	/// @brief
	/// @param world
	PhysicsWorldSystem::PhysicsWorldSystem(game::World& world)
	: game::WorldSystem(world)
	, _physicsWorld(Physics::GetInstance()->CreateWorld())
	{
	}

	/// @brief
	PhysicsWorldSystem::~PhysicsWorldSystem()
	{
		DefaultAllocator::GetInstance().Delete(_physicsWorld);
	}

	/// @brief
	/// @return
	const game::WorldSystemType& PhysicsWorldSystem::GetType()
	{
		static const game::WorldSystemType type {"Physics", +[](game::World& world) -> game::WorldSystem* {
			return DefaultAllocator::GetInstance().New<PhysicsWorldSystem>(world);
		}};
		return type;
	}

	/// @brief
	/// @param world
	/// @return
	PhysicsWorldSystem* PhysicsWorldSystem::Get(const game::World& world)
	{
		return static_cast<PhysicsWorldSystem*>(world.FindSystem(GetType()));
	}

	/// @brief
	/// @param world
	/// @return
	World* PhysicsWorldSystem::GetPhysicsWorld(const game::World& world)
	{
		PhysicsWorldSystem* system = Get(world);
		return system != nullptr ? system->_physicsWorld : nullptr;
	}

	/// @brief
	/// @return
	World* PhysicsWorldSystem::GetPhysicsWorld() const
	{
		return _physicsWorld;
	}

	/// @brief
	/// @param fixedDeltaTime
	void PhysicsWorldSystem::OnFixedUpdate(float fixedDeltaTime)
	{
		_physicsWorld->Update(fixedDeltaTime);
	}
}
