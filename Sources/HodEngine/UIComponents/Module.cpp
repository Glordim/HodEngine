#include "HodEngine/UIComponents/Pch.hpp"
#include <HodEngine/Core/Module.hpp>
#include "HodEngine/UIComponents/Components/UIComponent.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline ui
{
	/// @brief The UIComponent (the factories, the UI types and the CanvasManager are UI's)
	HOD_STARTUP_MODULE(UIComponents, registration)
	{
		GameRegistrar gameRegistrar(registration);
		gameRegistrar.RegisterComponent<UIComponent>();
		return true;
	}
}
