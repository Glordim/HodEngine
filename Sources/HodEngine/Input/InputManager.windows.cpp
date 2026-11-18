#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputManager.hpp"
// #include "HodEngine/Input/API/RawInput/RawInputApi.hpp"
#include "HodEngine/Input/API/GameInput/GameInputApi.hpp"
#include "HodEngine/Input/API/WindowLib/WindowLibApi.hpp"
#include "HodEngine/Input/API/XInput/XInputApi.hpp"

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
		// The gamepads: GameInput, XInput without its runtime
		if (CreateApi<GameInputApi>() == false)
		{
			OUTPUT_MESSAGE("Gamepads through XInput");
			if (CreateApi<XInputApi>() == false)
			{
				return false;
			}
		}
		return true;
	}
}
