#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Keyboard.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API KeyboardWindowLib : public Keyboard
	{
	public:
											KeyboardWindowLib();
											KeyboardWindowLib(const KeyboardWindowLib&) = delete;
											KeyboardWindowLib(KeyboardWindowLib&&) = delete;
											~KeyboardWindowLib() override = default;

		KeyboardWindowLib&					operator = (const KeyboardWindowLib&) = delete;
		KeyboardWindowLib&					operator = (KeyboardWindowLib&&) = delete;

		void								OnKeyPressed(int keycode);
		void								OnKeyReleased(int keycode);

	protected:

		bool								ApplyFeedback(Feedback& feedback) override;
	};
}
