#include "HodEngine/InputEditor/Pch.hpp"
#include "HodEngine/InputEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/InputEditor/Window/InputManagerWindow.hpp"

#include "HodEngine/Editor/EditorRegistrar.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

HOD_STARTUP_EDITOR_MODULE(InputEditor, registration)
{
	hod::editor::EditorRegistrar editorRegistrar(registration);
	editorRegistrar.RegisterWindow<InputManagerWindow>("");
	return 0;
}
