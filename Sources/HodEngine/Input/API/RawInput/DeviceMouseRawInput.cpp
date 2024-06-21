#include "HodEngine/Input/API/RawInput/DeviceMouseRawInput.hpp"
#include "HodEngine/Input/Api.hpp"

#include <WinUser.h>

namespace hod::input
{
	/// @brief 
	/// @param handle 
	/// @param name 
	/// @param  
	DeviceMouseRawInput::DeviceMouseRawInput(HANDLE handle, const std::string_view& name, const RID_DEVICE_INFO_MOUSE&)
	: DeviceMouse(ComputeDeviceUID(handle), name, Product::UNKNOWN)
	, _handle(handle)
	, _button0(GetSystemMetrics(SM_SWAPBUTTON) == 0 ? InputId::MouseButtonLeft : InputId::MouseButtonRight) // Check Left-handed mode
	, _button1(GetSystemMetrics(SM_SWAPBUTTON) == 0 ? InputId::MouseButtonRight : InputId::MouseButtonLeft) // Check Left-handed mode
	{
		// Position
		AddInput(&_axisX);
		AddInput(&_axisY);

		// Wheel
		AddInput(&_wheel);

		// Buttons
		AddInput(&_button0);
		AddInput(&_button1);
		AddInput(&_button2);
		AddInput(&_button3);
		AddInput(&_button4);

		SetConnected(true);
	}

	/// @brief 
	/// @param feedback 
	/// @return 
	bool DeviceMouseRawInput::ApplyFeedback(Feedback& feedback)
	{
		return false;
	}

	/// @brief 
	/// @param cusorPositionX 
	/// @param cursorPositionY 
	void DeviceMouseRawInput::ResyncLastCusorPosition()
	{
		_lastAbsoluteDirty = true;
	}

	/// @brief 
	void DeviceMouseRawInput::PrepareUpdate()
	{
		SetInputValue(_axisX, 0.0f);
		SetInputValue(_axisY, 0.0f);

		SetInputValue(_wheel, 0.0f);
	}

	/// @brief 
	/// @param rawMouse 
	void DeviceMouseRawInput::ReadRawInput(const RAWMOUSE& rawMouse)
	{
		if ((rawMouse.usFlags & MOUSE_MOVE_ABSOLUTE) != 0)
		{
			bool isVirtualDesktop = (rawMouse.usFlags & MOUSE_VIRTUAL_DESKTOP) != 0;

			int32_t width = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
			int32_t height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

			int32_t absoluteX = static_cast<int32_t>((rawMouse.lLastX / 65535.0f) * width);
			int32_t absoluteY = static_cast<int32_t>((rawMouse.lLastY / 65535.0f) * height);

			if (_lastAbsoluteDirty == true)
			{
				_lastAbsoluteDirty = false;
				SetInputValue(_axisX, 0.0f);
				SetInputValue(_axisY, 0.0f);
			}
			else
			{
				SetInputValue(_axisX, _axisX.GetValue() + (float)(absoluteX - _lastAbsoluteX));
				SetInputValue(_axisY, _axisY.GetValue() + (float)(absoluteY - _lastAbsoluteY));
			}

			_lastAbsoluteX = absoluteX;
			_lastAbsoluteY = absoluteY;
		}
		else
		{
			SetInputValue(_axisX, _axisX.GetValue() + (float)rawMouse.lLastX);
			SetInputValue(_axisY, _axisY.GetValue() + (float)rawMouse.lLastY);
		}

		USHORT uiButtonFlag = rawMouse.usButtonFlags;

		if (uiButtonFlag & RI_MOUSE_WHEEL)
		{
			SetInputValue(_wheel, static_cast<float>((SHORT)rawMouse.usButtonData) / (float)WHEEL_DELTA);
		}

		if (uiButtonFlag & RI_MOUSE_BUTTON_1_DOWN)
		{
			SetInputValue(_button0, 1.0f);
		}
		else if (uiButtonFlag & RI_MOUSE_BUTTON_1_UP)
		{
			SetInputValue(_button0, 0.0f);
		}

		if (uiButtonFlag & RI_MOUSE_BUTTON_2_DOWN)
		{
			SetInputValue(_button1, 1.0f);
		}
		else if (uiButtonFlag & RI_MOUSE_BUTTON_2_UP)
		{
			SetInputValue(_button1, 0.0f);
		}

		if (uiButtonFlag & RI_MOUSE_BUTTON_3_DOWN)
		{
			SetInputValue(_button2, 1.0f);
		}
		else if (uiButtonFlag & RI_MOUSE_BUTTON_3_UP)
		{
			SetInputValue(_button2, 0.0f);
		}

		if (uiButtonFlag & RI_MOUSE_BUTTON_4_DOWN)
		{
			SetInputValue(_button3, 1.0f);
		}
		else if (uiButtonFlag & RI_MOUSE_BUTTON_4_UP)
		{
			SetInputValue(_button3, 0.0f);
		}

		if (uiButtonFlag & RI_MOUSE_BUTTON_5_DOWN)
		{
			SetInputValue(_button4, 1.0f);
		}
		else if (uiButtonFlag & RI_MOUSE_BUTTON_5_UP)
		{
			SetInputValue(_button4, 0.0f);
		}
	}

	/// @brief 
	/// @return 
	HANDLE DeviceMouseRawInput::GetHandle()
	{
		return _handle;
	}

	/// @brief 
	/// @param hDevice 
	/// @return 
	UID DeviceMouseRawInput::ComputeDeviceUID(HANDLE hDevice)
	{
		return UID(Api::DeviceUidOffset::RAW_INPUT, reinterpret_cast<uint64_t>(hDevice));
	}
}
