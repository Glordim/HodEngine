#include <HodEngine/Game/ComponentFactory.hpp>

#include "Components/BarComponent.hpp"
#include "Components/PlayerControllerComponent.hpp"

extern "C"
{
	__declspec(dllexport) int Init();
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
