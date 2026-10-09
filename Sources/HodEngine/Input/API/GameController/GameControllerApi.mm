#include "HodEngine/Input/API/GameController/GameControllerApi.hpp"
#include "HodEngine/Input/API/GameController/GameControllerGamepad.hpp"
#include "HodEngine/Input/Pch.hpp"


#include "HodEngine/Input/InputManager.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#import <Foundation/Foundation.h>
#import <GameController/GameController.h>

namespace hod::inline input {
/// @brief
GameControllerApi::GameControllerApi() : Api("GameController") {}

/// @brief
/// @return
bool GameControllerApi::Initialize() {
  _connectObserver =
      (__bridge_retained void *)[[NSNotificationCenter defaultCenter]
          addObserverForName:GCControllerDidConnectNotification
                      object:nil
                       queue:[NSOperationQueue mainQueue]
                  usingBlock:^(NSNotification *_Nonnull note) {
                    GCController *controller = note.object;
                    if (controller.extendedGamepad != nil) {
                      AddPadDevice(controller.extendedGamepad);
                    }
                    printf("[+] Manette connectée : %s\n",
                           controller.vendorName.UTF8String);
                  }];

  _disconnectObserver =
      (__bridge_retained void *)[[NSNotificationCenter defaultCenter]
          addObserverForName:GCControllerDidDisconnectNotification
                      object:nil
                       queue:[NSOperationQueue mainQueue]
                  usingBlock:^(NSNotification *_Nonnull note) {
                    GCController *controller = note.object;
                    if (controller.extendedGamepad != nil) {
                      RemovePadDevice(controller.extendedGamepad);
                    }
                    printf("[-] Manette déconnectée : %s\n",
                           controller.vendorName.UTF8String);
                  }];

  return true;
}

/// @brief
GameControllerApi::~GameControllerApi() {
  if (_connectObserver != nil) {
    [[NSNotificationCenter defaultCenter]
        removeObserver:(__bridge id)_connectObserver];
  }
  if (_disconnectObserver != nil) {
    [[NSNotificationCenter defaultCenter]
        removeObserver:(__bridge id)_disconnectObserver];
  }
}

void GameControllerApi::AddPadDevice(GCExtendedGamepad *extendedGamepad) {
  GameControllerGamepad *pad =
      DefaultAllocator::GetInstance().New<GameControllerGamepad>(
          extendedGamepad);

  _pads.push_back(pad);
}

void GameControllerApi::RemovePadDevice(GCExtendedGamepad *extendedGamepad) {
  auto it =
      std::find_if(_pads.Begin(), _pads.End(),
                   [extendedGamepad](GameControllerGamepad *pad) {
                     return (pad->GetInternalExtendedPad() == extendedGamepad);
                   });
  if (it != _pads.End()) {
    DefaultAllocator::GetInstance().Delete(*it);
    _pads.Erase(it);
  }
}

/// @brief
void GameControllerApi::UpdateDeviceValues() {
  for (GameControllerGamepad *pad : _pads) {
    pad->WriteNextState();
    pad->UpdateState();
  }
}
}
