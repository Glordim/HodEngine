#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputManager.hpp"

#include "HodEngine/Input/API/Android/AndroidInput.hpp"

namespace hod::inline input
{
	/// @brief 
	/// @return 
	bool InputManager::InitializeApis()
	{
		if (CreateApi<AndroidInput>() == false)
		{
			return false;
		}
		return true;
	}
}
