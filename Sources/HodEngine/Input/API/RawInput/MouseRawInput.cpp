#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/RawInput/MouseRawInput.hpp"

#include <Windows.h>
#include <WinUser.h>

#include <algorithm>

#undef max

namespace hod::input
{
	struct MouseState : public State
	{
		int16_t _delta[2];
		int8_t  _wheel;
		uint8_t _buttons;
	};

	// GetSystemMetrics(SM_SWAPBUTTON) == 0 ? InputId::MouseButtonLeft : InputId::MouseButtonRight

	/// @brief
	/// @param handle
	/// @param name
	/// @param
	MouseRawInput::MouseRawInput(HANDLE handle, const std::string_view& name, const tagRID_DEVICE_INFO_MOUSE&)
	: Mouse(ComputeDeviceUID(handle), name, Product::UNKNOWN)
	, _handle(handle)
	{
		_axisX.SetStateView(StateView(StateView::Format::S16, 0));
		_axisY.SetStateView(StateView(StateView::Format::S16, 2));

		_axisWheel.SetStateView(StateView(StateView::Format::S8, 4));

		_buttonLeft.SetStateView(StateView(StateView::Format::Bit, 5, 0));
		_buttonRight.SetStateView(StateView(StateView::Format::Bit, 5, 1));
		_buttonWheel.SetStateView(StateView(StateView::Format::Bit, 5, 2));
		_buttonForward.SetStateView(StateView(StateView::Format::Bit, 5, 4));
		_buttonBackward.SetStateView(StateView(StateView::Format::Bit, 5, 3));

		SetConnected(true);
	}

	/// @brief
	/// @param feedback
	/// @return
	bool MouseRawInput::ApplyFeedback(Feedback& /*feedback*/)
	{
		return false;
	}

	/// @brief
	/// @param cusorPositionX
	/// @param cursorPositionY
	void MouseRawInput::ResyncLastCusorPosition()
	{
		_lastAbsoluteDirty = true;
	}

	void MouseRawInput::ResetNextState()
	{
		EditNextState<MouseState>()->_delta[0] = 0;
		EditNextState<MouseState>()->_delta[1] = 0;
		EditNextState<MouseState>()->_wheel = 0;
	}

	/// @brief
	/// @param rawMouse
	void MouseRawInput::ReadRawInput(const tagRAWMOUSE& rawMouse)
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
				EditNextState<MouseState>()->_delta[0] = 0;
				EditNextState<MouseState>()->_delta[1] = 0;
			}
			else
			{
				EditNextState<MouseState>()->_delta[0] = (int16_t)std::clamp((int32_t)EditNextState<MouseState>()->_delta[0] + (absoluteX - _lastAbsoluteX),
				                                                             (int32_t)std::numeric_limits<int16_t>::lowest(), (int32_t)std::numeric_limits<int16_t>::max());
				EditNextState<MouseState>()->_delta[1] = (int16_t)std::clamp((int32_t)EditNextState<MouseState>()->_delta[1] + (absoluteY - _lastAbsoluteY),
				                                                             (int32_t)std::numeric_limits<int16_t>::lowest(), (int32_t)std::numeric_limits<int16_t>::max());
			}

			_lastAbsoluteX = absoluteX;
			_lastAbsoluteY = absoluteY;
		}
		else
		{
			EditNextState<MouseState>()->_delta[0] = (int16_t)std::clamp((int32_t)EditNextState<MouseState>()->_delta[0] + (int32_t)rawMouse.lLastX,
			                                                             (int32_t)std::numeric_limits<int16_t>::lowest(), (int32_t)std::numeric_limits<int16_t>::max());
			EditNextState<MouseState>()->_delta[1] = (int16_t)std::clamp((int32_t)EditNextState<MouseState>()->_delta[1] + (int32_t)rawMouse.lLastY,
			                                                             (int32_t)std::numeric_limits<int16_t>::lowest(), (int32_t)std::numeric_limits<int16_t>::max());
		}

		USHORT uiButtonFlag = rawMouse.usButtonFlags;

		if (uiButtonFlag & RI_MOUSE_WHEEL)
		{
			int32_t detents = EditNextState<MouseState>()->_wheel;
			detents += (SHORT)rawMouse.usButtonData / WHEEL_DELTA;
			EditNextState<MouseState>()->_wheel = (int8_t)std::clamp(detents, (int32_t)std::numeric_limits<int8_t>::lowest(), (int32_t)std::numeric_limits<int8_t>::max());
			MarkForCurrent();
		}

		if (uiButtonFlag & RI_MOUSE_LEFT_BUTTON_DOWN)
		{
			EditNextState<MouseState>()->_buttons |= (1 << 0);
			MarkForCurrent();
		}
		else if (uiButtonFlag & RI_MOUSE_LEFT_BUTTON_UP)
		{
			EditNextState<MouseState>()->_buttons &= ~(1 << 0);
		}

		if (uiButtonFlag & RI_MOUSE_RIGHT_BUTTON_DOWN)
		{
			EditNextState<MouseState>()->_buttons |= (1 << 1);
			MarkForCurrent();
		}
		else if (uiButtonFlag & RI_MOUSE_RIGHT_BUTTON_UP)
		{
			EditNextState<MouseState>()->_buttons &= ~(1 << 1);
		}

		if (uiButtonFlag & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		{
			EditNextState<MouseState>()->_buttons |= (1 << 2);
			MarkForCurrent();
		}
		else if (uiButtonFlag & RI_MOUSE_MIDDLE_BUTTON_UP)
		{
			EditNextState<MouseState>()->_buttons &= ~(1 << 2);
		}

		if (uiButtonFlag & RI_MOUSE_BUTTON_4_DOWN)
		{
			EditNextState<MouseState>()->_buttons |= (1 << 3);
			MarkForCurrent();
		}
		else if (uiButtonFlag & RI_MOUSE_BUTTON_4_UP)
		{
			EditNextState<MouseState>()->_buttons &= ~(1 << 3);
		}

		if (uiButtonFlag & RI_MOUSE_BUTTON_5_DOWN)
		{
			EditNextState<MouseState>()->_buttons |= (1 << 4);
			MarkForCurrent();
		}
		else if (uiButtonFlag & RI_MOUSE_BUTTON_5_UP)
		{
			EditNextState<MouseState>()->_buttons &= ~(1 << 4);
		}
	}

	/// @brief
	/// @return
	HANDLE MouseRawInput::GetHandle()
	{
		return _handle;
	}

	/// @brief
	/// @param hDevice
	/// @return
	UID MouseRawInput::ComputeDeviceUID(HANDLE hDevice)
	{
		return UID(Api::DeviceUidOffset::RAW_INPUT, reinterpret_cast<uint64_t>(hDevice));
	}
}
