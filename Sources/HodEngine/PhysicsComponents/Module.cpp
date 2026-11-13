#include "HodEngine/PhysicsComponents/Pch.hpp"
#include "HodEngine/PhysicsComponents/Module.hpp"

#include <HodEngine/Game/GameRegistrar.hpp>
#include "HodEngine/PhysicsComponents/PhysicsWorldSystem.hpp"

#include "HodEngine/PhysicsComponents/Components/BoxCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/CapsuleCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/CircleCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Collider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/EdgeCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/MultiShapeCollider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Rigidbody2dComponent.hpp"

#include <HodEngine/Physics/Physics.hpp>

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace
{
	void RegisterPhysicsModule(hod::ModuleRegistration& registration)
	{
		hod::game::GameRegistrar gameRegistrar(registration);
		gameRegistrar.RegisterComponent<hod::physics::Collider2dComponent>();
		gameRegistrar.RegisterComponent<hod::physics::MultiShapeCollider2dComponent>();
		gameRegistrar.RegisterComponent<hod::physics::BoxCollider2dComponent>();
		gameRegistrar.RegisterComponent<hod::physics::CircleCollider2dComponent>();
		gameRegistrar.RegisterComponent<hod::physics::CapsuleCollider2dComponent>();
		gameRegistrar.RegisterComponent<hod::physics::EdgeCollider2dComponent>();
		gameRegistrar.RegisterComponent<hod::physics::Rigidbody2dComponent>();

		gameRegistrar.RegisterWorldSystem(hod::physics::PhysicsWorldSystem::GetType());
	}

	bool InitPhysicsModule()
	{
		return hod::physics::Physics::CreatePhysicsInstance()->Init();
	}

	void ShutdownPhysicsModule()
	{
		hod::physics::Physics::DestroyInstance();
	}
}

HOD_DEFINE_RUNTIME_MODULE(PhysicsComponents, &RegisterPhysicsModule, &InitPhysicsModule, &ShutdownPhysicsModule)
