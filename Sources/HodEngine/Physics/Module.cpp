#include "HodEngine/Physics/Pch.hpp"
#include <HodEngine/Core/Module.hpp>
#include "HodEngine/Physics/Physics.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline physics
{
	/// @brief The physics engine (its components are PhysicsComponents')
	HOD_STARTUP_MODULE(Physics, registration)
	{
		Physics* physics = Physics::CreatePhysicsInstance();
		registration.AddUnregister([]() { Physics::DestroyInstance(); });
		return physics->Init();
	}
}
