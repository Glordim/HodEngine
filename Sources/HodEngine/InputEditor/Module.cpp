#include "HodEngine/InputEditor/Pch.hpp"
#include "HodEngine/InputEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/InputEditor/Window/InputManagerWindow.hpp"

#include <HodEngine/Editor/WindowFactory.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

HOD_STARTUP_MODULE(InputEditor)
{
	WindowFactory::GetInstance()->Register<InputManagerWindow>("");

	return 0;
}

HOD_SHUTDOWN_MODULE(InputEditor)
{
	//WindowFactory::GetInstance()->Register<>(); // todo

	return 0;
}
