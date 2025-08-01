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
		std::memset(_nextState, false, sizeof(_nextState));
	}

	bool DeviceKeyboardWindowLib::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void DeviceKeyboardWindowLib::OnKeyPressed(int keycode)
	{
		_nextState[keycode] = true;
	}

	void DeviceKeyboardWindowLib::OnKeyReleased(int keycode)
	{
		_nextState[keycode] = false;
	}
}
