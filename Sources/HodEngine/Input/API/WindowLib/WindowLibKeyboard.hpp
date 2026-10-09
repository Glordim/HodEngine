#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Keyboard.hpp"

#include <HodEngine/Window/ScanCode.hpp>

namespace hod::inline input
{
	/// @brief
	class HOD_INPUT_API WindowLibKeyboard : public Keyboard
	{
	public:
		WindowLibKeyboard();
		WindowLibKeyboard(const WindowLibKeyboard&) = delete;
		WindowLibKeyboard(WindowLibKeyboard&&) = delete;
		~WindowLibKeyboard() override = default;

		WindowLibKeyboard& operator=(const WindowLibKeyboard&) = delete;
		WindowLibKeyboard& operator=(WindowLibKeyboard&&) = delete;

		void OnKeyPressed(ScanCode scanCode);
		void OnKeyReleased(ScanCode scanCode);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;
	};
}
