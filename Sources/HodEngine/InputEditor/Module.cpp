#include "HodEngine/InputEditor/Pch.hpp"
#include <HodEngine/Editor/EditorModule.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/InputEditor/Window/InputManagerWindow.hpp"

#include "HodEngine/Editor/EditorRegistrar.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline input
{
	HOD_STARTUP_EDITOR_MODULE(InputEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		editorRegistrar.RegisterWindow<InputManagerWindow>("");
		return true;
	}
}
