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

namespace hod::inline physics
{
	namespace
	{
		void RegisterPhysicsModule(ModuleRegistration& registration)
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

		bool InitPhysicsModule()
		{
			return Physics::CreatePhysicsInstance()->Init();
		}

		void ShutdownPhysicsModule()
		{
			Physics::DestroyInstance();
		}
	}

	HOD_DEFINE_RUNTIME_MODULE(PhysicsComponents, &RegisterPhysicsModule, &InitPhysicsModule, &ShutdownPhysicsModule)
}
