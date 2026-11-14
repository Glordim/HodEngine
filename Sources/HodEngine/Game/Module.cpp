#include "HodEngine/Game/Pch.hpp"
#include <HodEngine/Core/Module.hpp>

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"
#include "HodEngine/Game/GameRegistrar.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline game
{
	/// @brief The factories the other plugins and the project's runtime module register into (Game is required, every plugin knowing it depends on it:
	/// started before them, stopped after them) and Game's own components
	HOD_STARTUP_MODULE(Game, registration)
	{
		ComponentFactory::CreateInstance();
		DataStructFactory::CreateInstance();
		registration.AddUnregister(
			[]()
			{
				DataStructFactory::DestroyInstance();
				ComponentFactory::DestroyInstance();
			});

		GameRegistrar gameRegistrar(registration); // Unregistered before the factories go
		gameRegistrar.RegisterComponent<NodeComponent>();
		gameRegistrar.RegisterComponent<Node2dComponent>();
		return true;
	}
}
