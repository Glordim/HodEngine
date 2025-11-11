#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputManager.hpp"
// #include "HodEngine/Input/API/RawInput/ApiRawInput.hpp"
#include "HodEngine/Input/API/WindowLib/ApiWindowLib.hpp"
#include "HodEngine/Input/API/XInput/ApiXInput.hpp"

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
		if (CreateApi<ApiXInput>() == false)
		{
			return false;
		}
		return true;
	}
}
