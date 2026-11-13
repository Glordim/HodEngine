#pragma once
#include <HodEngine/HodEngine.hpp>

namespace hod::inline core
{
	class ModuleRegistration;
}

/// @brief The entry point of an editor library, the same in each one (always a DLL)
#define HOD_EDITOR_MODULE_STARTUP_FUNCTION_NAME "StartupEditorModule"

/// @brief The entry point of the editor library Name (see ModuleStartup: an engine module's, started by Editor::LoadEditorModules, or a
/// game's, by Project::LoadProjectModules), returning false if it couldn't start: registers what it adds through the registrars
/// (EditorRegistrar, GameEditorRegistrar...). Undone before the library unloads (Hot Reload included): what has to be undone is pushed on
/// the registration (ModuleRegistration::AddUnregister), there is no shutdown. Usable in the library's namespace (extern "C")
#define HOD_STARTUP_EDITOR_MODULE(Name, Registration) extern "C" HOD_EXPORT bool StartupEditorModule(::hod::core::ModuleRegistration& Registration)
