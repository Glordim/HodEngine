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

namespace hod::inline input
{
	/// @brief 
	class HOD_INPUT_API GameControllerGamepad : public Gamepad
	{
	public:
											GameControllerGamepad(GCExtendedGamepad* extendedGamepad);
											GameControllerGamepad(const GameControllerGamepad&) = delete;
											GameControllerGamepad(GameControllerGamepad&&) = delete;
											~GameControllerGamepad() override = default;

		GameControllerGamepad&				operator = (const GameControllerGamepad&) = delete;
		GameControllerGamepad&				operator = (GameControllerGamepad&&) = delete;

		GCExtendedGamepad*					GetInternalExtendedPad() const;

		void								WriteNextState();

	protected:

		bool								ApplyFeedback(Feedback& feedback) override;

	private:

		GCExtendedGamepad*					_extendedGamepad;
	};
}
