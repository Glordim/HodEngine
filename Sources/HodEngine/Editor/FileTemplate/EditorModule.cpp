#include "[[MODULE_NAME]]Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/Editor/EditorRegistrar.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

/// @brief What the game adds to the editor, through the registrars of the modules offering it (EditorRegistrar: drawers, windows...;
/// GameEditorRegistrar: viewport debug views...), unregistered by the editor when the module goes away (Hot Reload included)
/// @param registration
/// @return
HOD_STARTUP_EDITOR_MODULE([[MODULE_NAME]], registration)
{
	hod::editor::EditorRegistrar editorRegistrar(registration);
	(void)editorRegistrar;
	return 0;
}
