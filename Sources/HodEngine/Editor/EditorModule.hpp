#pragma once
#include <HodEngine/HodEngine.hpp>

namespace hod::inline core
{
	class ModuleRegistration;
}

/// @brief The entry point of an editor library, the same in each one (always a DLL)
#define HOD_EDITOR_MODULE_STARTUP_FUNCTION_NAME "StartupEditorModule"

/// @brief The entry point of the editor library Name (an engine module's, see Editor::LoadEditorModules, or a game's, see
/// Project::LoadProjectModules): registers what it adds through the registrars (EditorRegistrar, GameEditorRegistrar...). The editor owns
/// the registration and undoes it before unloading the library (Hot Reload included): what has to be undone is pushed on it
/// (ModuleRegistration::AddUnregister), there is no shutdown. Usable in the library's namespace (extern "C")
#define HOD_STARTUP_EDITOR_MODULE(Name, Registration) extern "C" HOD_EXPORT void StartupEditorModule(::hod::core::ModuleRegistration& Registration)
