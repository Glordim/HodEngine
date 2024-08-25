#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/API/RawInput/ApiRawInput.hpp"
#include "HodEngine/Input/API/XInput/ApiXInput.hpp"

namespace hod::input
{
	/// @brief 
	/// @return 
	bool InputManager::InitializeApis(window::Window* window)
	{
		if (CreateApi<ApiRawInput>(window) == false)
		{
			return false;
		}
		if (CreateApi<ApiXInput>(window) == false)
		{
			return false;
		}
		return true;
	}
}
