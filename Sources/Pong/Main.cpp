#include <HodEngine/Game/ComponentFactory.hpp>

#include "Components/BarComponent.hpp"
#include "Components/PlayerControllerComponent.hpp"

extern "C"
{
	__declspec(dllexport) int Init();
	__declspec(dllexport) int Clean();
}

/// @brief 
/// @return 
int Init()
{
	hod::game::ComponentFactory* componentFactory = hod::game::ComponentFactory::GetInstance();
	componentFactory->Register<BarComponent>();
	componentFactory->Register<PlayerControllerComponent>();
	return 0;
}

/// @brief 
/// @return 
int Clean()
{
	hod::game::ComponentFactory* componentFactory = hod::game::ComponentFactory::GetInstance();
	componentFactory->Unregister<BarComponent>();
	componentFactory->Unregister<PlayerControllerComponent>();
	return 0;
}
