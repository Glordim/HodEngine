#include <limits>
#include <array>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/WindowLib/KeyboardWindowLib.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Core/Type.hpp"

#undef max

namespace hod::input
{
	KeyboardWindowLib::KeyboardWindowLib()
	: Keyboard(UID::INVALID_UID, "Keyboard", Product::UNKNOWN)
	{
	}

	bool KeyboardWindowLib::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void KeyboardWindowLib::OnKeyPressed(int keycode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (keycode / 8));
		*bytePtr |= (1 << (keycode % 8));
	}

	void KeyboardWindowLib::OnKeyReleased(int keycode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (keycode / 8));
		*bytePtr &= ~(1 << (keycode % 8));
	}
}
