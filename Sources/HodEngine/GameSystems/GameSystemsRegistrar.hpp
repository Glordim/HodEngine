#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include "HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp"

#include <HodEngine/Core/ModuleRegistration.hpp>

namespace hod::inline gamesystems
{
	/// @brief The registrar of GameSystems (see ModuleRegistration): project settings, unregistered with the rest of what the module
	/// registered. In a runtime module's startup (see HOD_STARTUP_MODULE): the settings are loaded after it
	class GameSystemsRegistrar
	{
	public:
		explicit GameSystemsRegistrar(ModuleRegistration& registration)
		: _registration(registration)
		{
		}

	public:
		/// @brief A ProjectSettings class (see ProjectSettingsManager): its file in <Project>/ProjectSettings, edited in Edit > Project Settings
		/// @return Its instance, loaded already if the settings were loaded before. Registered already (by another module): that one,
		/// unregistered with the module that registered it, not taken over
		template<typename _Settings_>
		_Settings_* RegisterProjectSettings();

	private:
		ModuleRegistration& _registration;
	};

	template<typename _Settings_>
	_Settings_* GameSystemsRegistrar::RegisterProjectSettings()
	{
		ProjectSettingsManager* projectSettingsManager = ProjectSettingsManager::GetInstance();
		if (_Settings_* settings = projectSettingsManager->Get<_Settings_>())
		{
			return settings;
		}

		_Settings_* settings = projectSettingsManager->Register<_Settings_>();
		_registration.AddUnregister(
			[]()
			{
				if (ProjectSettingsManager* projectSettingsManager = ProjectSettingsManager::GetInstance())
				{
					projectSettingsManager->Unregister<_Settings_>();
				}
			});
		return settings;
	}
}
