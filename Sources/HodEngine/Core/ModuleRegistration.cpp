#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/ModuleRegistration.hpp"

namespace hod::inline core
{
	/// @brief
	ModuleRegistration::~ModuleRegistration()
	{
		UnregisterAll();
	}

	/// @brief
	/// @param unregister
	void ModuleRegistration::AddUnregister(std::function<void()> unregister)
	{
		_unregisters.PushBack(std::move(unregister));
	}

	/// @brief
	void ModuleRegistration::UnregisterAll()
	{
		while (_unregisters.Empty() == false)
		{
			std::function<void()> unregister = std::move(_unregisters.Back());
			_unregisters.PopBack();
			unregister();
		}
	}
}
