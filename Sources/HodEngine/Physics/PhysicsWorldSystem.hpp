#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <HodEngine/Game/WorldSystem.hpp>

namespace hod::inline physics
{
	class World;

	/// @brief The physics simulation of a game::World: steps its physics World each fixed step, after the scenes' FixedUpdate
	class HOD_PHYSICS_API PhysicsWorldSystem : public game::WorldSystem
	{
	public:
		explicit PhysicsWorldSystem(game::World& world);
		~PhysicsWorldSystem() override;

	public:
		static const game::WorldSystemType& GetType();

		/// @return nullptr if the world has no physics (the Physics module isn't registered)
		static PhysicsWorldSystem* Get(const game::World& world);

		/// @return nullptr if the world has no physics (the Physics module isn't registered)
		static World* GetPhysicsWorld(const game::World& world);

		World* GetPhysicsWorld() const;

		void OnFixedUpdate(float fixedDeltaTime) override;

	private:
		World* _physicsWorld = nullptr;
	};
}
