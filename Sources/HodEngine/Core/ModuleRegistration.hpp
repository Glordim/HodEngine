#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <functional>

namespace hod::inline core
{
	/// @brief What a module (a library of a plugin with an entry point: runtime or editor, see HOD_STARTUP_MODULE; a game's
	/// included) registered elsewhere, undone before its code goes away (Hot Reload included): one
	/// stack of unregisters, run in the reverse order of the registrations.
	/// It knows nothing of what is registered: each module offering registrations has a registrar, a facade over a ModuleRegistration
	/// doing the registration and pushing what undoes it (<Module>Registrar in HodEngine/<Module>/<Module>Registrar.hpp: GameRegistrar,
	/// UIRegistrar, EditorRegistrar...):
	///   game::GameRegistrar game(registration);
	///   game.RegisterComponent<Circle>();
	class HOD_CORE_API ModuleRegistration
	{
	public:
		ModuleRegistration() = default;
		ModuleRegistration(const ModuleRegistration&) = delete;
		ModuleRegistration(ModuleRegistration&&) = delete;
		~ModuleRegistration();

		ModuleRegistration& operator=(const ModuleRegistration&) = delete;
		ModuleRegistration& operator=(ModuleRegistration&&) = delete;

	public:
		/// @brief What undoes a registration done just before (by a registrar, or by the module itself: its own factories...)
		void AddUnregister(std::function<void()> unregister);

		/// @brief In the reverse order of AddUnregister
		void UnregisterAll();

	private:
		Vector<std::function<void()>> _unregisters;
	};
}
