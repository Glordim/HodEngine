#include <limits>
#include <array>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/GameController/DevicePadGameController.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Core/Type.hpp"

#undef max

namespace hod::input
{
	DevicePadGameController::DevicePadGameController(GCExtendedGamepad* extendedGamepad)
	: DevicePad(UID::INVALID_UID, "Gamepad", Product::UNKNOWN)
	, _extendedGamepad(extendedGamepad)
	{
		std::memset(&_nextState, false, sizeof(_nextState));
		std::memset(&_currentState, false, sizeof(_currentState));
		std::memset(&_previousState, false, sizeof(_previousState));
	}

	bool DevicePadGameController::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	GCExtendedGamepad* DevicePadGameController::GetInternalExtendedPad() const
	{
		return _extendedGamepad;
	}

	const State* DevicePadGameController::GetState() const
	{
		return &_currentState;
	}

	uint32_t DevicePadGameController::GetStateSize() const
	{
		return sizeof(_currentState);
	}

	void DevicePadGameController::UpdateState()
	{
		_nextState._buttons[0] = 0;
		if (_extendedGamepad.leftShoulder.pressed)
			_nextState._buttons[0] |= (1 << 0);
		if (_extendedGamepad.rightShoulder.pressed)
			_nextState._buttons[0] |= (1 << 1);
		if (_extendedGamepad.leftThumbstickButton.pressed)
			_nextState._buttons[0] |= (1 << 2);
		if (_extendedGamepad.rightThumbstickButton.pressed)
			_nextState._buttons[0] |= (1 << 3);

		std::memcpy(&_previousState, &_currentState, sizeof(_currentState));
		_stateLock.Lock();
		std::memcpy(&_currentState, &_nextState, sizeof(_currentState));
		_stateLock.Unlock();
	}
}
