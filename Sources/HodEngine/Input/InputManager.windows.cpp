#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/API/XInput/ApiXInput.hpp"

namespace hod::input
{
	/// @brief 
	/// @return 
	bool InputManager::InitializeApis()
	{
		if (CreateApi<ApiXInput>() == false)
		{
			return false;
		}
		return true;
	}
}
