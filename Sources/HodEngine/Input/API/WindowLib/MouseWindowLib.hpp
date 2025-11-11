#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Mouse.hpp"

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

		void OnButtonPressed(int button);
		void OnButtonReleased(int button);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;
	};
}
