#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/GameController/ApiGameController.hpp"
#include "HodEngine/Input/API/GameController/DevicePadGameController.hpp"

#include "HodEngine/Input/InputManager.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#import <Foundation/Foundation.h>
#import <GameController/GameController.h>

namespace hod::input
{
	/// @brief 
	ApiGameController::ApiGameController()
	: Api("GameController")
	{
	}

	/// @brief 
	/// @return 
	bool ApiGameController::Initialize()
	{
		_connectObserver = (__bridge_retained void*)[[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidConnectNotification
                                            	object:nil
                                                queue:[NSOperationQueue mainQueue]
                                                usingBlock:^(NSNotification* _Nonnull note)
		{
            GCController* controller = note.object;
			if (controller.extendedGamepad != nil)
			{
				AddPadDevice(controller.extendedGamepad);
			}
            printf("[+] Manette connectée : %s\n", controller.vendorName.UTF8String);
        }];

		_disconnectObserver = (__bridge_retained void*)[[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidDisconnectNotification
                                                object:nil
                                            	queue:[NSOperationQueue mainQueue]
                                                usingBlock:^(NSNotification* _Nonnull note)
		{
            GCController* controller = note.object;
			if (controller.extendedGamepad != nil)
			{
				RemovePadDevice(controller.extendedGamepad);
			}
            printf("[-] Manette déconnectée : %s\n", controller.vendorName.UTF8String);
        }];

		return true;
	}

	/// @brief 
	ApiGameController::~ApiGameController()
	{
		if (_connectObserver != nil)
		{
    		[[NSNotificationCenter defaultCenter] removeObserver:(__bridge id)_connectObserver];
		}
		if (_disconnectObserver != nil)
		{
			[[NSNotificationCenter defaultCenter] removeObserver:(__bridge id)_disconnectObserver];
		}
	}

	void ApiGameController::AddPadDevice(GCExtendedGamepad* extendedGamepad)
	{
		DevicePadGameController* pad = DefaultAllocator::GetInstance().New<DevicePadGameController>(extendedGamepad);

		_pads.push_back(pad);
	}

	void ApiGameController::RemovePadDevice(GCExtendedGamepad* extendedGamepad)
	{
		auto it = std::find_if(_pads.begin(), _pads.end(), [extendedGamepad](DevicePadGameController* pad){
			return (pad->GetInternalExtendedPad() == extendedGamepad);
		});
		if (it != _pads.end())
		{
			DefaultAllocator::GetInstance().Delete(*it);
			_pads.erase(it);
		}
	}

	/// @brief 
	void ApiGameController::UpdateDeviceValues()
	{
		for (DevicePadGameController* pad : _pads)
		{
			pad->UpdateState();
		}
	}
}
