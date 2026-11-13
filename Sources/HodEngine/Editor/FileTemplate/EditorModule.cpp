#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/Core/Module.hpp>
#include <HodEngine/Editor/EditorRegistrar.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

/// @brief What the game adds to the editor, through the registrars of the modules offering it (EditorRegistrar: drawers, windows...;
/// GameEditorRegistrar: viewport debug views...), unregistered by the editor when the module goes away (Hot Reload included)
/// @param registration
/// @return false if it couldn't start
HOD_STARTUP_MODULE([[MODULE_NAME]], registration)
{
	hod::editor::EditorRegistrar editorRegistrar(registration);
	(void)editorRegistrar;
	return true;
}
