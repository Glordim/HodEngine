#include "[[MODULE_NAME]]Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>

#include "Components/MyFirstComponent.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace
{
	/// @brief The game's types, through the registrars of the modules offering them (GameRegistrar: components, data structs, world
	/// systems; UIRegistrar: views...), unregistered by the engine when the module goes away (Hot Reload included)
	/// @param registration
	void Register(hod::ModuleRegistration& registration)
	{
		hod::game::GameRegistrar gameRegistrar(registration);
		gameRegistrar.RegisterComponent<MyFirstComponent>();
	}
}

HOD_DEFINE_RUNTIME_MODULE([[MODULE_NAME]], &Register)
