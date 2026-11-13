#pragma once

namespace hod::inline core
{
	class ModuleRegistration;
}

// The entry points of an editor module (an engine module's, see Editor::LoadEditorModules, or a game's, see Project::LoadProjectModules): the
// editor owns the module's ModuleRegistration, given to its startup (registering through the registrars, EditorRegistrar...), and unregisters
// all of it once its shutdown (optional) has run, before unloading it. HOD_STARTUP/SHUTDOWN_EDITOR_MODULE are usable in the module's namespace
// (namespace hod::inline audio { ... }): extern "C", the same functions as the declared ones.
#define HOD_DECLARE_EDITOR_MODULE(Name, API)                                              \
	extern "C"                                                                            \
	{                                                                                     \
		API int StartupEditorModule(hod::core::ModuleRegistration& registration);        \
		API int ShutdownEditorModule();                                                   \
	}

#define HOD_STARTUP_EDITOR_MODULE(Name, Registration) extern "C" int StartupEditorModule(::hod::core::ModuleRegistration& Registration)
#define HOD_SHUTDOWN_EDITOR_MODULE(Name) extern "C" int ShutdownEditorModule()
