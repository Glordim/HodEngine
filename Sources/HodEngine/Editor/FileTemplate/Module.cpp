#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/Core/RuntimeModule.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>

#include "Components/MyFirstComponent.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

/// @brief The game's types, through the registrars of the modules offering them (GameRegistrar: components, data structs, world systems;
/// UIRegistrar: views...; GameSystemsRegistrar: project settings), unregistered by the engine when the module goes away (Hot Reload
/// included). Services to start once the project settings are loaded: registration.AddInit(init, shutdown)
/// @param registration
HOD_STARTUP_RUNTIME_MODULE([[MODULE_NAME]], registration)
{
	hod::game::GameRegistrar gameRegistrar(registration);
	gameRegistrar.RegisterComponent<MyFirstComponent>();
}
