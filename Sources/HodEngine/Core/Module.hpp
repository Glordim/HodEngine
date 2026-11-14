#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/Core/ModuleRegistration.hpp> // What the entry point is given

/// @brief The entry point of a module loaded from its library (see ModuleHost::Load), the same in each one, runtime or editor: the
/// identifier (defined by HOD_STARTUP_MODULE) and its name (looked for by ModuleHost::Load), one from the other
#define HOD_MODULE_STARTUP_FUNCTION HodStartupModule
#define HOD_MODULE_STRINGIFY_(x) #x
#define HOD_MODULE_STRINGIFY(x) HOD_MODULE_STRINGIFY_(x)
#define HOD_MODULE_STARTUP_FUNCTION_NAME HOD_MODULE_STRINGIFY(HOD_MODULE_STARTUP_FUNCTION)
#define HOD_MODULE_CONCAT_(a, b) a##b
#define HOD_MODULE_CONCAT(a, b) HOD_MODULE_CONCAT_(a, b)

/// @brief The entry point of the module Name, runtime (Physics, PhysicsComponents, a project's <Name>) or editor (PhysicsEditor, a project's
/// <Name>Editor), see ModuleStartup: started by a ModuleHost in its plugin's order, the project settings loaded before, returning false if
/// it couldn't start. It registers what it adds through the registrars (GameRegistrar, EditorRegistrar...), starts its services (it may
/// read its settings) and pushes on the registration what undoes them: there is no shutdown, it's undone before its code goes (Hot Reload
/// included). Usable in the library's namespace (extern "C").
/// - Its own library (a DLL: the editor): the exported HOD_MODULE_STARTUP_FUNCTION (HodStartupModule), Name unused. Found in the module
///   itself first, before its dependencies (which have theirs): GetProcAddress, dlsym (the library first), RTLD_FIRST on macOS
/// - HOD_MONOLITHIC (an application build, everything in one binary): HOD_MODULE_STARTUP_FUNCTION_OF(Name) (HodStartupModule_Physics), unique, not
///   exported, declared and called by the generated code with these macros (HodModules.generated.cpp, see
///   hod_add_engine_modules_entry_points)
#if defined(HOD_MONOLITHIC)
	/// @brief The entry point of the module Name, linked in (its name only exists in a monolithic build)
	#define HOD_MODULE_STARTUP_FUNCTION_OF(Name) HOD_MODULE_CONCAT(HOD_MODULE_STARTUP_FUNCTION, _##Name)
	#define HOD_STARTUP_MODULE(Name, Registration) extern "C" bool HOD_MODULE_STARTUP_FUNCTION_OF(Name)(::hod::core::ModuleRegistration& registration)
#else
	#define HOD_STARTUP_MODULE(Name, Registration) extern "C" HOD_EXPORT bool HOD_MODULE_STARTUP_FUNCTION(::hod::core::ModuleRegistration& registration)
#endif
