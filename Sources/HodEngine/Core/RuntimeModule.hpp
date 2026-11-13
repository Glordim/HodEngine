#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/ModuleHost.hpp"

/// @brief The entry point of a runtime module loaded from a DLL (the editor), the same in each one
#define HOD_RUNTIME_MODULE_STARTUP_FUNCTION_NAME "StartupRuntimeModule"

/// @brief The entry point of the runtime module Name (see ModuleStartup, started by a ModuleHost in its plugin's order, the project settings
/// loaded before), returning false if it couldn't start: registers its types through the registrars, starts its services (it may read its
/// settings), pushes on the registration what stops them. Usable in the library's namespace (extern "C").
/// - A DLL: the exported StartupRuntimeModule, found by the editor in each module (in the module itself first, before its dependencies)
/// - HOD_MONOLITHIC (a game build, everything in one binary): HodRuntimeModule_<Name>, unique, not exported, called by the generated code
///   (HodModules.generated.cpp, see hod_add_engine_modules_entry_points)
#if defined(HOD_MONOLITHIC)
	#define HOD_STARTUP_RUNTIME_MODULE(Name, Registration) extern "C" bool HodRuntimeModule_##Name(::hod::core::ModuleRegistration& Registration)
#else
	#define HOD_STARTUP_RUNTIME_MODULE(Name, Registration) extern "C" HOD_EXPORT bool StartupRuntimeModule(::hod::core::ModuleRegistration& Registration)
#endif
