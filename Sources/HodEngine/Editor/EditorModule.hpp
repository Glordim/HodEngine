#pragma once

namespace hod::inline editor
{
	class EditorModuleRegistration;
}

// The entry points of an editor module (see Editor::LoadEditorModules), in place of HOD_DECLARE_MODULE: the editor owns the module's
// EditorModuleRegistration, given to its startup, and unregisters all of it once its shutdown (optional) has run, before unloading it.
#define HOD_DECLARE_EDITOR_MODULE(Name, API)                                                       \
	extern "C"                                                                                     \
	{                                                                                              \
		API int StartupEditorModule(hod::editor::EditorModuleRegistration& registration);          \
		API int ShutdownEditorModule();                                                            \
	}

#define HOD_STARTUP_EDITOR_MODULE(Name, Registration) int StartupEditorModule(hod::editor::EditorModuleRegistration& Registration)
#define HOD_SHUTDOWN_EDITOR_MODULE(Name) int ShutdownEditorModule()
