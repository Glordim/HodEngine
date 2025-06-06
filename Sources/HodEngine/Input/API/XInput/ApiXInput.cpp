#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/XInput/ApiXInput.hpp"
#include "HodEngine/Input/API/XInput/DevicePadXbox.hpp"

namespace hod::input
{
	/// @brief 
	ApiXInput::ApiXInput() : Api("XInput")
	{
	}

	/// @brief 
	ApiXInput::~ApiXInput()
	{
		for (DevicePadXbox* device : _pads)
		{
			if (device != nullptr)
			{
				if (device->IsConnected() == true)
				{
					Api::NotifyDeviceDisconnected(device);
				}

				DefaultAllocator::GetInstance().Delete(device);
			}
		}

		if (_hInstance != nullptr)
		{
			FreeLibrary(_hInstance);
		}
	}

	/// @brief 
	/// @return 
	bool ApiXInput::Initialize()
	{
		_hInstance = LoadLibrary(TEXT("xinput1_4.dll"));
		if (_hInstance == nullptr)
		{
			_hInstance = LoadLibrary(TEXT("xinput9_1_0.dll"));
			if (_hInstance == nullptr)
			{
				_hInstance = LoadLibrary(TEXT("xinput1_3.dll"));
				if (_hInstance == nullptr)
				{
					return false;
				}
			}
		}

		_getStateProc = (XInputGetStateProc)GetProcAddress(_hInstance, "XInputGetState");
		_setStateProc = (XInputSetStateProc)GetProcAddress(_hInstance, "XInputSetState");

		for (uint32_t padIndex = 0; padIndex < MaxPad; ++padIndex)
		{
			DevicePadXbox* device = DefaultAllocator::GetInstance().New<DevicePadXbox>(this, padIndex);

			_pads[padIndex] = device;

			AddDevice(device);
		}

		SetInitialized(true);
		return true;
	}

	/// @brief 
	void ApiXInput::UpdateDeviceValues()
	{
		for (DevicePadXbox* xboxPad : _pads)
		{
			bool wasConnected = xboxPad->IsConnected();
			bool isConnected = xboxPad->Update();
			if (wasConnected != isConnected)
			{
				if (isConnected == true)
				{
					Api::NotifyDeviceConnected(xboxPad);
				}
				else
				{
					Api::NotifyDeviceDisconnected(xboxPad);
				}
			}
		}
	}

	/// @brief 
	/// @param padIndex 
	/// @param state 
	/// @return 
	bool ApiXInput::GetPadState(uint32_t padIndex, XINPUT_STATE* state) const
	{
		return _getStateProc(padIndex, state) == ERROR_SUCCESS;
	}

	/// @brief 
	/// @param padIndex 
	/// @param vibration 
	/// @return 
	bool ApiXInput::SetPadState(uint32_t padIndex, XINPUT_VIBRATION* vibration) const
	{
		return _setStateProc(padIndex, vibration) == ERROR_SUCCESS;
	}
}
