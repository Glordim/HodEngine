#include <limits>

#include "HodEngine/Core/TypeTrait.hpp"
#include "HodEngine/Input/API/GameController/GameControllerGamepad.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"
#include "HodEngine/Input/Pch.hpp"

#import <GameController/GameController.h>

#undef max

namespace hod::inline input {
struct PadGameControllerState : public State {
  uint8_t _buttons[16]; // 1 button = 1 bit
};

GameControllerGamepad::GameControllerGamepad(GCExtendedGamepad *extendedGamepad)
    : Gamepad(UID::INVALID_UID, "Gamepad", Product::UNKNOWN,
              sizeof(PadGameControllerState)),
      _extendedGamepad(extendedGamepad) {}

bool GameControllerGamepad::ApplyFeedback(Feedback &feedback) {
  (void)feedback;
  return false;
}

GCExtendedGamepad *GameControllerGamepad::GetInternalExtendedPad() const {
  return _extendedGamepad;
}

void GameControllerGamepad::WriteNextState() {
  EditNextState<PadGameControllerState>()->_buttons[0] = 0;
  if (_extendedGamepad.leftShoulder.pressed)
    EditNextState<PadGameControllerState>()->_buttons[0] |= (1 << 0);
  if (_extendedGamepad.rightShoulder.pressed)
    EditNextState<PadGameControllerState>()->_buttons[0] |= (1 << 1);
  if (_extendedGamepad.leftThumbstickButton.pressed)
    EditNextState<PadGameControllerState>()->_buttons[0] |= (1 << 2);
  if (_extendedGamepad.rightThumbstickButton.pressed)
    EditNextState<PadGameControllerState>()->_buttons[0] |= (1 << 3);
}
} // namespace hod::inline input
