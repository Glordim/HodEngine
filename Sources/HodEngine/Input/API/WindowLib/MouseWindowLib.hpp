#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Mouse.hpp"

#include <HodEngine/Window/MouseButton.hpp>

namespace hod::input
{
	/// @brief
	class HOD_INPUT_API MouseWindowLib : public Mouse
	{
	public:
		MouseWindowLib();
		MouseWindowLib(const MouseWindowLib&) = delete;
		MouseWindowLib(MouseWindowLib&&) = delete;
		~MouseWindowLib() override = default;

		MouseWindowLib& operator=(const MouseWindowLib&) = delete;
		MouseWindowLib& operator=(MouseWindowLib&&) = delete;

		void OnButtonPressed(window::MouseButton button);
		void OnButtonReleased(window::MouseButton button);
		void OnButtonMoved(int x, int y);
		void OnButtonScroll(int scroll);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;
		void ResetNextState() override;

		int16_t _lastPosX = 0;
		int16_t _lastPosY = 0;
	};
}
