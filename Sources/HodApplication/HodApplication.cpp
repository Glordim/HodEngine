#include "HodApplication.hpp"

#include <HodEngine/Editor/Editor.hpp>
#include <HodEngine/Game/ComponentFactory.hpp>

#include "Components/BarComponent.hpp"
#include "Components/PlayerControllerComponent.hpp"

_SingletonOverrideConstructor(HodApplication)
{

}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool HodApplication::Init(const hod::ArgumentParser& argumentParser)
{
	bool platformApplicationResult = PlatformApplication::Init(argumentParser);
	if (platformApplicationResult == false)
	{
		return false;
	}

	hod::game::ComponentFactory* componentFactory = hod::game::ComponentFactory::GetInstance();
	componentFactory->Register<BarComponent>();
	componentFactory->Register<PlayerControllerComponent>();

#if defined(HOD_EDITOR)
	hod::editor::Editor::CreateInstance();
	if (hod::editor::Editor::GetInstance()->Init(argumentParser) == false)
	{
		return false;
	}
#else
	// Start game
#endif

	return true;
}
