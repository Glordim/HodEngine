#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/DevicePad.hpp"
#include "HodEngine/Input/InputButton.hpp"
#include "HodEngine/Input/State.hpp"

#include <HodEngine/Core/Concurency/SpinLock.hpp>

#ifdef __OBJC__
@class GCExtendedGamepad;
#else
class GCExtendedGamepad;
#endif

namespace hod::input
{
	struct PadState : public State
	{
		uint8_t _buttons[16]; // 1 button = 1 bit
		
	};

	/// @brief 
	class HOD_INPUT_API DevicePadGameController : public DevicePad
	{
	public:
											DevicePadGameController(GCExtendedGamepad* extendedGamepad);
											DevicePadGameController(const DevicePadGameController&) = delete;
											DevicePadGameController(DevicePadGameController&&) = delete;
											~DevicePadGameController() override = default;

		DevicePadGameController&			operator = (const DevicePadGameController&) = delete;
		DevicePadGameController&			operator = (DevicePadGameController&&) = delete;

		const State*						GetState() const override;
		uint32_t							GetStateSize() const override;

		void								UpdateState() override;

		GCExtendedGamepad*					GetInternalExtendedPad() const;

	protected:

		bool								ApplyFeedback(Feedback& feedback) override;

	private:

		GCExtendedGamepad*					_extendedGamepad;

		SpinLock							_stateLock;
		PadState							_nextState;
		PadState							_currentState;
		PadState							_previousState;
	};
}
