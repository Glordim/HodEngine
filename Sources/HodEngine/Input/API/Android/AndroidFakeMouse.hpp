#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Mouse.hpp"

#include <HodEngine/Window/MouseButton.hpp>

namespace hod::inline input
{
	/// @brief
	class HOD_INPUT_API AndroidFakeMouse : public Mouse
	{
	public:
		AndroidFakeMouse();
		AndroidFakeMouse(const AndroidFakeMouse&) = delete;
		AndroidFakeMouse(AndroidFakeMouse&&) = delete;
		~AndroidFakeMouse() override = default;

		AndroidFakeMouse& operator=(const AndroidFakeMouse&) = delete;
		AndroidFakeMouse& operator=(AndroidFakeMouse&&) = delete;

		void OnButtonPressed(MouseButton button);
		void OnButtonReleased(MouseButton button);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;
		void ResetNextState() override;
	};
}
