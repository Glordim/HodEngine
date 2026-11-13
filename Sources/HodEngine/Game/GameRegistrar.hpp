#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include <HodEngine/Core/ModuleRegistration.hpp>

#include <type_traits>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline game
{
	struct WorldSystemType;

	/// @brief The registrar of Game (see ModuleRegistration): components, data structs, world systems, unregistered with the rest of what
	/// the module registered. In a runtime module's startup (see HOD_STARTUP_MODULE). What is registered already (by another module) is
	/// refused with an error (false): the first one stays, nothing is undone for this one.
	class HOD_GAME_API GameRegistrar
	{
	public:
		explicit GameRegistrar(ModuleRegistration& registration);

	public:
		template<typename _Component_>
		bool RegisterComponent();
		/// @return false if registered already
		bool RegisterComponent(ReflectionDescriptor& descriptor);

		/// @brief A DataStruct type (what the data items and data tables contain)
		/// @return false (with an error) if registered already
		template<typename _DataStruct_>
		bool RegisterDataStruct();

		/// @return false (with an error) if registered already
		bool RegisterWorldSystem(const WorldSystemType& type);

	private:
		ModuleRegistration& _registration;
	};

	template<typename _Component_>
	bool GameRegistrar::RegisterComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component");
		return RegisterComponent(_Component_::GetReflectionDescriptor());
	}

	template<typename _DataStruct_>
	bool GameRegistrar::RegisterDataStruct()
	{
		if (DataStructFactory::GetInstance()->Register<_DataStruct_>() == false)
		{
			return false;
		}
		_registration.AddUnregister(
			[]()
			{
				if (DataStructFactory* dataStructFactory = DataStructFactory::GetInstance())
				{
					dataStructFactory->Unregister<_DataStruct_>();
				}
			});
		return true;
	}
}
