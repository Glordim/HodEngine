#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"

#include <win32/windows_base.h>

extern "C"
{
#define MAPVK_VK_TO_VSC (0)
#define MAPVK_VSC_TO_VK (1)
#define MAPVK_VK_TO_CHAR (2)
#define MAPVK_VSC_TO_VK_EX (3)
#define MAPVK_VK_TO_VSC_EX (4)

	typedef void* HKL;

	UINT WINAPI MapVirtualKeyExA(_In_ UINT uCode, _In_ UINT uMapType, _In_opt_ HKL dwhkl);
	HKL WINAPI  GetKeyboardLayout(_In_ DWORD idThread);
}

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
			UINT virtualKey = MapVirtualKeyExA(scanCode, MAPVK_VSC_TO_VK_EX, hKeyboardLayout);

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
			UINT scanCode = MapVirtualKeyExA(virtualKey, MAPVK_VK_TO_VSC_EX, hKeyboardLayout);

			return static_cast<InputId>(scanCode + static_cast<UINT>(InputId::KeyboardStartEnum));
		}

		return inputId;
	}
}
