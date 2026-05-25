#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;

HOD_STARTUP_MODULE(UI2Editor)
{
	return 0;
}

HOD_SHUTDOWN_MODULE(UI2Editor)
{
	return 0;
}
