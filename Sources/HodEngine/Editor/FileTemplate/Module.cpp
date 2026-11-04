#include "[[MODULE_NAME]]Module.hpp"
#include <HodEngine/Game/ComponentFactory.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "Components/MyFirstComponent.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

/// @brief 
/// @return 
HOD_STARTUP_MODULE([[MODULE_NAME]])
{
	hod::ComponentFactory* componentFactory = hod::ComponentFactory::GetInstance();
	componentFactory->Register<MyFirstComponent>();
	return 0;
}

/// @brief 
/// @return 
HOD_SHUTDOWN_MODULE([[MODULE_NAME]])
{
	hod::ComponentFactory* componentFactory = hod::ComponentFactory::GetInstance();
	componentFactory->Unregister<MyFirstComponent>();
	return 0;
}
