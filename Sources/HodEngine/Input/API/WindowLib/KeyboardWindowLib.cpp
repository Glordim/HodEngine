#include <array>
#include <limits>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/WindowLib/KeyboardWindowLib.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"

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

	void KeyboardWindowLib::OnKeyPressed(window::ScanCode scanCode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (std::to_underlying(scanCode) / 8));
		*bytePtr |= (1 << (std::to_underlying(scanCode) % 8));
	}

	void KeyboardWindowLib::OnKeyReleased(window::ScanCode scanCode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (std::to_underlying(scanCode) / 8));
		*bytePtr &= ~(1 << (std::to_underlying(scanCode) % 8));
	}
}
