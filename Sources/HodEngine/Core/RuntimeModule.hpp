#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/ModuleRegistration.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include <functional>

namespace hod::inline core
{
	/// @brief The ModuleRegistration of a runtime library (see HOD_STARTUP_RUNTIME_MODULE): what it registers, plus the services it starts
	/// once every library is registered and the project settings are loaded (AddInit)
	class HOD_CORE_API RuntimeModuleRegistration : public ModuleRegistration
	{
		friend class RuntimeModuleHost;

	public:
		/// @brief Run after the startup of every library and the load of the project settings (it may read them), in the libraries' order.
		/// If it succeeds, shutdown (optional) is pushed as an unregister: undone before what was registered
		/// @param init false: failed (with an error), the libraries' Init phase stops there
		void AddInit(std::function<bool()> init, std::function<void()> shutdown = nullptr);

	private:
		struct Init
		{
			std::function<bool()> _init;
			std::function<void()> _shutdown;
		};
		Vector<Init> _inits;
	};

	/// @brief The entry point of a runtime library (see HOD_STARTUP_RUNTIME_MODULE)
	using RuntimeModuleStartup = void (*)(RuntimeModuleRegistration& registration);

	/// @brief A runtime library and its entry point (the list compiled in a game, see HodModules.cpp.in)
	struct RuntimeModuleEntry
	{
		const char*          _name = nullptr;
		RuntimeModuleStartup _startup = nullptr;
	};

	/// @brief Runs the runtime libraries of the modules (an engine module's base and Components layer, Physics then PhysicsComponents; a
	/// game module): Application for the ones compiled in a game, the editor for the libraries of the project's modules. In the modules'
	/// order (dependencies first, the game last):
	/// - their startup right away (Register): types only, through the registrars (GameRegistrar, UIRegistrar...), no service started
	/// - (the project settings are loaded)
	/// - InitAll: the inits they asked for (the services, may read the project settings, may fail)
	/// - UnregisterAll: each library undone, in the reverse order (its shutdowns, then what it registered)
	class HOD_CORE_API RuntimeModuleHost
	{
	public:
		RuntimeModuleHost() = default;
		RuntimeModuleHost(const RuntimeModuleHost&) = delete;
		RuntimeModuleHost(RuntimeModuleHost&&) = delete;
		~RuntimeModuleHost();

		RuntimeModuleHost& operator=(const RuntimeModuleHost&) = delete;
		RuntimeModuleHost& operator=(RuntimeModuleHost&&) = delete;

	public:
		/// @brief Its startup, right away. In the libraries' order (dependencies first)
		void Register(const String& name, RuntimeModuleStartup startup);

		/// @brief The inits not run yet, in the libraries' order. Stops at the first that fails: the ones run already stay initialized,
		/// undone by UnregisterAll
		/// @return false (with an error naming the library) if one failed
		bool InitAll();

		/// @brief Each library undone, in the reverse order: its shutdowns (of the inits that succeeded), then what it registered
		void UnregisterAll();

		uint32_t GetModuleCount() const;

	private:
		struct Entry
		{
			String                     _name;
			RuntimeModuleRegistration* _registration = nullptr;
			uint32_t                   _initCount = 0; // Of its inits, the ones run already
		};
		Vector<Entry> _entries;
	};
}

/// @brief The entry point of a runtime library loaded as a plugin (a DLL: the editor), the same in each one
#define HOD_RUNTIME_MODULE_STARTUP_FUNCTION_NAME "StartupRuntimeModule"

/// @brief The entry point of the runtime library Name (see RuntimeModuleHost): registers its types through the registrars, asks for its
/// services' init (RuntimeModuleRegistration::AddInit). Usable in the library's namespace (extern "C").
/// - A DLL: the exported StartupRuntimeModule, found by the editor in each library (a library first, before its dependencies)
/// - HOD_MONOLITHIC (a game build, everything in one binary): HodRuntimeModule_<Name>, unique, not exported, called by the generated code
///   (HodModules.generated.cpp, see hod_add_engine_modules_entry_points)
#if defined(HOD_MONOLITHIC)
	#define HOD_STARTUP_RUNTIME_MODULE(Name, Registration) extern "C" void HodRuntimeModule_##Name(::hod::core::RuntimeModuleRegistration& Registration)
#else
	#define HOD_STARTUP_RUNTIME_MODULE(Name, Registration) extern "C" HOD_EXPORT void StartupRuntimeModule(::hod::core::RuntimeModuleRegistration& Registration)
#endif
