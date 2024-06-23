#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/API/RawInput/ApiRawInput.hpp"
#include "HodEngine/Input/API/XInput/ApiXInput.hpp"

namespace hod::input
{
	/// @brief 
	/// @return 
	bool InputManager::InitializeApis()
	{
		if (CreateApi<ApiRawInput>() == false)
		{
			return false;
		}
		if (CreateApi<ApiXInput>() == false)
		{
			return false;
		}
		return true;
	}
}
