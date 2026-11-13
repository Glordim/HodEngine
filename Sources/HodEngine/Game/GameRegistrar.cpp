#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/GameRegistrar.hpp"
#include "HodEngine/Game/WorldSystem.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline game
{
	/// @brief
	/// @param registration
	GameRegistrar::GameRegistrar(ModuleRegistration& registration)
	: _registration(registration)
	{
	}

	/// @brief
	/// @param descriptor
	/// @return
	bool GameRegistrar::RegisterComponent(ReflectionDescriptor& descriptor)
	{
		if (ComponentFactory::GetInstance()->Register(descriptor) == false)
		{
			return false;
		}
		_registration.AddUnregister(
			[&descriptor]()
			{
				if (ComponentFactory* componentFactory = ComponentFactory::GetInstance())
				{
					componentFactory->Unregister(descriptor);
				}
			});
		return true;
	}

	/// @brief
	/// @param type
	/// @return
	bool GameRegistrar::RegisterWorldSystem(const WorldSystemType& type)
	{
		if (WorldSystemRegistry::Register(type) == false)
		{
			return false;
		}
		_registration.AddUnregister([&type]() { WorldSystemRegistry::Unregister(type); });
		return true;
	}
}
