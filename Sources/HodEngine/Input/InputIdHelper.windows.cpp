#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"

#include <Windows.h>

namespace hod::input
{
	/// @brief 
	/// @param inputId 
	/// @return 
	InputId InputIdHelper::PhysicalToVirtual(InputId inputId)
	{
		if (inputId > InputId::KeyboardStartEnum && inputId < InputId::KeyboardEndEnum)
		{
			HKL hKeyboardLayout = GetKeyboardLayout(0);

			UINT scanCode = static_cast<UINT>(inputId) - static_cast<UINT>(InputId::KeyboardStartEnum);
			UINT virtualKey = MapVirtualKeyEx(scanCode, MAPVK_VSC_TO_VK_EX, hKeyboardLayout);

			InputId virtualInputId = static_cast<InputId>(InputId::KeyboardVirtualStartEnum + (virtualKey - 'A') + 1);
			if (virtualInputId > InputId::KeyboardVirtualStartEnum && virtualInputId < InputId::KeyboardVirtualEndEnum)
			{
				return virtualInputId;
			}
		}

		return inputId;
	}
	
	/// @brief 
	/// @param inputId 
	/// @return 
	InputId InputIdHelper::VirtualToPhysical(InputId inputId)
	{
		if (inputId > InputId::KeyboardVirtualStartEnum && inputId < InputId::KeyboardVirtualEndEnum)
		{
			HKL hKeyboardLayout = GetKeyboardLayout(0);

			UINT virtualKey = static_cast<UINT>(inputId) - static_cast<UINT>(InputId::KeyboardVirtualStartEnum) + 'A' - 1;
			UINT scanCode = MapVirtualKeyEx(virtualKey, MAPVK_VK_TO_VSC_EX, hKeyboardLayout);

			return static_cast<InputId>(scanCode + static_cast<UINT>(InputId::KeyboardStartEnum));
		}

		return inputId;
	}
}
