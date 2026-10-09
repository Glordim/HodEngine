#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/API/WindowLib/WindowLibApi.hpp"
#include "HodEngine/Input/API/GameController/GameControllerApi.hpp"

namespace hod::inline input
{
	/// @brief 
	/// @return 
	bool InputManager::InitializeApis()
	{
		if (CreateApi<WindowLibApi>() == false)
		{
			return false;
		}

		if (CreateApi<GameControllerApi>() == false)
		{
			return false;
		}
		return true;
	}
}
