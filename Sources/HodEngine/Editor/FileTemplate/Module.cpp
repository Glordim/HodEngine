#include "[[PROJECT_NAME]].hpp"
#include <HodEngine/Game/ComponentFactory.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "Components/MyFirstComponent.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

/// @brief 
/// @return 
int Init()
{
	hod::game::ComponentFactory* componentFactory = hod::game::ComponentFactory::GetInstance();
	componentFactory->Register<MyFirstComponent>();
	return 0;
}

/// @brief 
/// @return 
int Clean()
{
	hod::game::ComponentFactory* componentFactory = hod::game::ComponentFactory::GetInstance();
	componentFactory->Unregister<MyFirstComponent>();
	return 0;
}
