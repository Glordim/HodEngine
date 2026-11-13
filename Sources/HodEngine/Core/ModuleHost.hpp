#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/ModuleRegistration.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

namespace hod::inline core
{
	/// @brief The entry point of a module (a library of a plugin), runtime or editor (see HOD_STARTUP_RUNTIME_MODULE,
	/// HOD_STARTUP_EDITOR_MODULE): registers through the registrars, starts its services, pushes on the registration what undoes them
	/// @return false (with an error): it couldn't start, what it registered already is undone
	using ModuleStartup = bool (*)(ModuleRegistration& registration);

	/// @brief Starts modules in order (their plugins' order: dependencies first) and stops them in the reverse order, each with its
	/// ModuleRegistration: Application for the runtime modules compiled in a game, the editor for the modules of the project's plugins (one
	/// host for the runtime ones, one for the editor ones), Project for the game module and its editor module. It doesn't load them (a game
	/// build has none to load).
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
		/// @return false (with an error naming the module) if it failed: what it registered is undone, it isn't kept
		bool Start(const String& name, ModuleStartup startup);

		/// @brief The last count modules started undone, in the reverse order (a plugin's, when its next one failed to start)
		void StopLast(uint32_t count);

		/// @brief Each module undone, in the reverse order (see ModuleRegistration)
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
