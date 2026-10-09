#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Mouse.hpp"

#include <HodEngine/Window/MouseButton.hpp>

namespace hod::inline input
{
	/// @brief
	class HOD_INPUT_API WindowLibMouse : public Mouse
	{
	public:
		WindowLibMouse();
		WindowLibMouse(const WindowLibMouse&) = delete;
		WindowLibMouse(WindowLibMouse&&) = delete;
		~WindowLibMouse() override = default;

		WindowLibMouse& operator=(const WindowLibMouse&) = delete;
		WindowLibMouse& operator=(WindowLibMouse&&) = delete;

		void OnButtonPressed(MouseButton button);
		void OnButtonReleased(MouseButton button);
		void OnButtonMoved(int x, int y);
		void OnButtonScroll(int scroll);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;
		void ResetNextState() override;

		int16_t _lastPosX = 0;
		int16_t _lastPosY = 0;
	};
}
