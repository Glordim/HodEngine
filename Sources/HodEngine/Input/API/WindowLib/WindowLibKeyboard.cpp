#include <limits>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Core/TypeTrait.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/WindowLib/WindowLibKeyboard.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"

#undef max

namespace hod::inline input
{
	WindowLibKeyboard::WindowLibKeyboard()
	: Keyboard(UID::INVALID_UID, "Keyboard", Product::UNKNOWN)
	{
	}

	bool WindowLibKeyboard::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void WindowLibKeyboard::OnKeyPressed(ScanCode scanCode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (std::to_underlying(scanCode) / 8));
		*bytePtr |= (1 << (std::to_underlying(scanCode) % 8));
	}

	void WindowLibKeyboard::OnKeyReleased(ScanCode scanCode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(EditNextState()) + (std::to_underlying(scanCode) / 8));
		*bytePtr &= ~(1 << (std::to_underlying(scanCode) % 8));
	}
}
