#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/API/WindowLib/ApiWindowLib.hpp"
#include "HodEngine/Input/API/GameController/ApiGameController.hpp"

namespace hod::input
{
	/// @brief 
	/// @return 
	bool InputManager::InitializeApis()
	{
		if (CreateApi<ApiWindowLib>() == false)
		{
			return false;
		}

		if (CreateApi<ApiGameController>() == false)
		{
			return false;
		}
		return true;
	}
}
