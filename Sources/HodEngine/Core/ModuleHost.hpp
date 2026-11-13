#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/ModuleRegistration.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

namespace hod::inline core
{
	/// @brief The entry point of a module's library, runtime or editor (see HOD_STARTUP_RUNTIME_MODULE, HOD_STARTUP_EDITOR_MODULE): registers
	/// through the registrars, starts its services, pushes on the registration what undoes them
	/// @return false (with an error): it couldn't start, what it registered already is undone
	using ModuleStartup = bool (*)(ModuleRegistration& registration);

	/// @brief Starts libraries of modules in order (dependencies first) and stops them in the reverse order, each with its ModuleRegistration:
	/// Application for the runtime libraries compiled in a game, the editor for the libraries of the project's modules (runtime, then
	/// editor), Project for the game's. It doesn't load them (a game build has none to load).
	class HOD_CORE_API ModuleHost
	{
	public:
		ModuleHost() = default;
		ModuleHost(const ModuleHost&) = delete;
		ModuleHost(ModuleHost&&) = delete;
		~ModuleHost();

		ModuleHost& operator=(const ModuleHost&) = delete;
		ModuleHost& operator=(ModuleHost&&) = delete;

	public:
		/// @brief Its startup, right away
		/// @return false (with an error naming the library) if it failed: what it registered is undone, it isn't kept
		bool Start(const String& name, ModuleStartup startup);

		/// @brief The last count libraries started undone, in the reverse order (a module whose next library failed to start)
		void StopLast(uint32_t count);

		/// @brief Each library undone, in the reverse order (see ModuleRegistration)
		void StopAll();

		uint32_t GetModuleCount() const;

	private:
		struct Entry
		{
			String              _name;
			ModuleRegistration* _registration = nullptr;
		};
		Vector<Entry> _entries;
	};
}
