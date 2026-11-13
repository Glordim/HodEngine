#include "HodEngine/PhysicsComponents/Pch.hpp"
#include <HodEngine/Core/RuntimeModule.hpp>
#include "HodEngine/PhysicsComponents/PhysicsWorldSystem.hpp"

#include "HodEngine/PhysicsComponents/Components/BoxCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/CapsuleCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/CircleCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Collider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/EdgeCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/MultiShapeCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Rigidbody2dComponent.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline physics
{
	/// @brief The components and the world system (the physics engine is Physics')
	HOD_STARTUP_RUNTIME_MODULE(PhysicsComponents, registration)
	{
		GameRegistrar gameRegistrar(registration);
		gameRegistrar.RegisterComponent<Collider2dComponent>();
		gameRegistrar.RegisterComponent<MultiShapeCollider2dComponent>();
		gameRegistrar.RegisterComponent<BoxCollider2dComponent>();
		gameRegistrar.RegisterComponent<CircleCollider2dComponent>();
		gameRegistrar.RegisterComponent<CapsuleCollider2dComponent>();
		gameRegistrar.RegisterComponent<EdgeCollider2dComponent>();
		gameRegistrar.RegisterComponent<Rigidbody2dComponent>();

		gameRegistrar.RegisterWorldSystem(PhysicsWorldSystem::GetType());
	}
}
