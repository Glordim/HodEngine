#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/GameInput/GameInputApi.hpp"
#include "HodEngine/Input/API/GameInput/GameInputGamepad.hpp"

#include <Windows.h>
#include <GameInput.h>

using namespace GameInput::v4;

namespace hod::inline input
{
	/// @brief What GameInput calls back
	struct GameInputApiCallbacks
	{
		/// @brief On a thread of GameInput, the gamepads connected already included (when registered)
		static void CALLBACK OnDeviceEvent(GameInputCallbackToken callbackToken, void* context, GameInputDeviceEvent deviceEvent, IGameInputDevice* device)
		{
			(void)callbackToken;
			if (deviceEvent == GameInputDeviceEventConnected || deviceEvent == GameInputDeviceEventDisconnected)
			{
				static_cast<GameInputApi*>(context)->QueueDeviceEvent(device, deviceEvent == GameInputDeviceEventConnected);
			}
		}
	};

	/// @brief
	GameInputApi::GameInputApi()
	: Api("GameInput")
	{
	}

	/// @brief
	GameInputApi::~GameInputApi()
	{
		if (_gameInput != nullptr)
		{
			if (_deviceCallbackToken != 0)
			{
				_gameInput->UnregisterCallback(_deviceCallbackToken); // Waits for the callback running, if any
			}

			for (const DeviceEvent& deviceEvent : _deviceEvents)
			{
				deviceEvent._device->Release();
			}

			for (GameInputGamepad* pad : _pads)
			{
				if (pad->IsConnected())
				{
					Api::NotifyDeviceDisconnected(pad);
				}
				DefaultAllocator::GetInstance().Delete(pad);
			}

			_gameInput->Release();
		}
	}

	/// @brief
	/// @return false without a GameInput runtime (neither the system's nor a GameInputRedist.dll next to the executable)
	bool GameInputApi::Initialize()
	{
		HRESULT result = GameInputCreate(&_gameInput);
		if (FAILED(result))
		{
			_gameInput = nullptr;
			OUTPUT_MESSAGE("GameInput unavailable (HRESULT 0x{:08X})", static_cast<uint32_t>(result));
			return false;
		}

		result = _gameInput->RegisterDeviceCallback(nullptr, GameInputKindGamepad, this, &GameInputApiCallbacks::OnDeviceEvent, &_deviceCallbackToken);
		if (FAILED(result))
		{
			OUTPUT_ERROR("GameInput: unable to register the device callback (HRESULT 0x{:08X})", static_cast<uint32_t>(result));
			_deviceCallbackToken = 0;
			return false; // The destructor releases _gameInput
		}

		SetInitialized(true);
		return true;
	}

	/// @brief
	/// @param device
	/// @param connected
	void GameInputApi::QueueDeviceEvent(IGameInputDevice* device, bool connected)
	{
		device->AddRef();

		_deviceEventsLock.Lock();
		_deviceEvents.push_back({device, connected});
		_deviceEventsLock.Unlock();
	}

	/// @brief
	void GameInputApi::ApplyDeviceEvents()
	{
		_deviceEventsLock.Lock();
		Vector<DeviceEvent> deviceEvents = std::move(_deviceEvents);
		_deviceEvents.Clear();
		_deviceEventsLock.Unlock();

		for (const DeviceEvent& deviceEvent : deviceEvents)
		{
			GameInputGamepad* pad = FindPad(deviceEvent._device);
			if (deviceEvent._connected)
			{
				if (pad == nullptr)
				{
					pad = DefaultAllocator::GetInstance().New<GameInputGamepad>(deviceEvent._device);
					_pads.push_back(pad);
					AddDevice(pad);
				}
				else
				{
					pad->SetGameInputDevice(deviceEvent._device); // Connected again, maybe through another IGameInputDevice
				}

				if (pad->IsConnected() == false)
				{
					Api::SetDeviceConnected(pad, true);
					Api::NotifyDeviceConnected(pad);
				}
			}
			else if (pad != nullptr && pad->IsConnected())
			{
				pad->ClearState();
				pad->UpdateState();
				Api::SetDeviceConnected(pad, false);
				Api::NotifyDeviceDisconnected(pad);
			}

			deviceEvent._device->Release();
		}
	}

	/// @brief
	/// @param device
	/// @return
	GameInputGamepad* GameInputApi::FindPad(IGameInputDevice* device) const
	{
		for (GameInputGamepad* pad : _pads)
		{
			if (pad->IsSameDevice(device))
			{
				return pad;
			}
		}
		return nullptr;
	}

	/// @brief
	void GameInputApi::UpdateDeviceValues()
	{
		ApplyDeviceEvents();

		for (GameInputGamepad* pad : _pads)
		{
			if (pad->IsConnected() == false)
			{
				continue;
			}

			IGameInputReading* reading = nullptr;
			if (SUCCEEDED(_gameInput->GetCurrentReading(GameInputKindGamepad, pad->GetGameInputDevice(), &reading)))
			{
				GameInputGamepadState state;
				if (reading->GetGamepadState(&state))
				{
					pad->WriteState(state);
				}
				reading->Release();
			}
			pad->UpdateState();
		}
	}
}
