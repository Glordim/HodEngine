#include "[[MODULE_NAME]]Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

/// @brief 
/// @return 
HOD_STARTUP_MODULE([[MODULE_NAME]])
{
	return 0;
}

/// @brief 
/// @return 
HOD_SHUTDOWN_MODULE([[MODULE_NAME]])
{
	return 0;
}
