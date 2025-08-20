#include <limits>
#include <array>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/WindowLib/DeviceKeyboardWindowLib.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Core/Type.hpp"

#undef max

namespace hod::input
{
	DeviceKeyboardWindowLib::DeviceKeyboardWindowLib()
	: DeviceKeyboard(UID::INVALID_UID, "Keyboard", Product::UNKNOWN)
	{
	}

	bool DeviceKeyboardWindowLib::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void DeviceKeyboardWindowLib::OnKeyPressed(int keycode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (keycode / 8));
		*bytePtr |= (1 << (keycode % 8));
	}

	void DeviceKeyboardWindowLib::OnKeyReleased(int keycode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (keycode / 8));
		*bytePtr &= ~(1 << (keycode % 8));
	}
}
