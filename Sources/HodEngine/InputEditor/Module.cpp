#include "HodEngine/InputEditor/Pch.hpp"
#include "HodEngine/InputEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/InputEditor/Window/InputManagerWindow.hpp"

#include "HodEngine/Editor/EditorModuleRegistration.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	EditorModuleRegistration registration;
}

HOD_STARTUP_MODULE(InputEditor)
{
	registration.RegisterWindow<InputManagerWindow>("");
	return 0;
}

HOD_SHUTDOWN_MODULE(InputEditor)
{
	registration.UnregisterAll();
	return 0;
}
