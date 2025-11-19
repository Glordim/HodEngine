#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Gamepad.hpp"
#include "HodEngine/Input/InputButton.hpp"
#include "HodEngine/Input/State.hpp"

#ifdef __OBJC__
@class GCExtendedGamepad;
#else
class GCExtendedGamepad;
#endif

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API GamepadGameController : public Gamepad
	{
	public:
											GamepadGameController(GCExtendedGamepad* extendedGamepad);
											GamepadGameController(const GamepadGameController&) = delete;
											GamepadGameController(GamepadGameController&&) = delete;
											~GamepadGameController() override = default;

		GamepadGameController&				operator = (const GamepadGameController&) = delete;
		GamepadGameController&				operator = (GamepadGameController&&) = delete;

		GCExtendedGamepad*					GetInternalExtendedPad() const;

		void								WriteNextState();

	protected:

		bool								ApplyFeedback(Feedback& feedback) override;

	private:

		GCExtendedGamepad*					_extendedGamepad;
	};
}
