#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/ModuleRegistration.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline game
{
	/// @brief The entry points of a runtime module (an optional engine feature, Physics...: in its Components layer, PhysicsComponents; or
	/// a game module), see HOD_DEFINE_RUNTIME_MODULE.
	/// Phases, each run for every module in the modules' order (dependencies first, the game last) before the next phase starts:
	/// - Register: types only, through the registrars of the modules offering them (GameRegistrar: components, data structs, world systems;
	///   UIRegistrar: views...; see ModuleRegistration), no side effect, can't fail, doesn't read the project settings
	/// - (the project settings are loaded)
	/// - Init: the module's services (a physics engine...), may read its project settings, may fail
	/// - Shutdown: in the reverse order, for the modules whose Init succeeded, then everything registered is unregistered (reverse order)
	struct RuntimeModule
	{
		const char* _name = nullptr;
		void (*_register)(ModuleRegistration& registration) = nullptr;
		bool (*_init)() = nullptr;     // Optional
		void (*_shutdown)() = nullptr; // Optional
	};

	/// @brief Runs the phases of the runtime modules (see RuntimeModule): Application for the ones compiled in a game, the editor for the
	/// libraries of the project's modules
	class HOD_GAME_API RuntimeModuleHost
	{
	public:
		RuntimeModuleHost() = default;
		RuntimeModuleHost(const RuntimeModuleHost&) = delete;
		RuntimeModuleHost(RuntimeModuleHost&&) = delete;
		~RuntimeModuleHost();

		RuntimeModuleHost& operator=(const RuntimeModuleHost&) = delete;
		RuntimeModuleHost& operator=(RuntimeModuleHost&&) = delete;

	public:
		/// @brief Its Register phase, right away. In the modules' order (dependencies first)
		void Register(const RuntimeModule& module);

		/// @brief The Init phase of every registered module, in their order. If one fails, the ones initialized already are shut down
		/// @return false (with an error naming the module) if one failed
		bool InitAll();

		/// @brief The Shutdown phase of the initialized modules, in the reverse order
		void ShutdownAll();

		/// @brief After ShutdownAll (done here otherwise): unregisters what the modules registered, in the reverse order
		void UnregisterAll();

		uint32_t GetModuleCount() const;

	private:
		struct Entry
		{
			const RuntimeModule*       _module = nullptr;
			ModuleRegistration*        _registration = nullptr;
			bool                       _initialized = false;
		};
		Vector<Entry> _entries;
	};
}

/// @brief The name of the function giving the entry points of the runtime module Name: unique per module, a static game links them all
#define HOD_RUNTIME_MODULE_FUNCTION(Name) HodRuntimeModule_##Name

/// @brief In the module's Module.hpp (or above the definition): the C function the editor finds in the module's library, and the
/// generated code of a static game calls
#define HOD_DECLARE_RUNTIME_MODULE(Name, API) \
	extern "C"                                \
	{                                         \
		API const hod::game::RuntimeModule* HOD_RUNTIME_MODULE_FUNCTION(Name)(); \
	}

/// @brief HOD_DEFINE_RUNTIME_MODULE(PhysicsComponents, &Register, &Init, &Shutdown): the functions as in RuntimeModule (Init and Shutdown optional)
#define HOD_DEFINE_RUNTIME_MODULE(Name, ...)                                         \
	const hod::game::RuntimeModule* HOD_RUNTIME_MODULE_FUNCTION(Name)()             \
	{                                                                                \
		static const hod::game::RuntimeModule runtimeModule {#Name, __VA_ARGS__};    \
		return &runtimeModule;                                                       \
	}
