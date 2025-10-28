#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/RawInput/ApiRawInput.hpp"

#include "HodEngine/Input/API/RawInput/KeyboardRawInput.hpp"
#include "HodEngine/Input/API/RawInput/MouseRawInput.hpp"
#include "HodEngine/Input/InputManager.hpp"

#include <hidusage.h>
#include <WinDNS.h>
#include <WinUser.h>

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp>
#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp>

using namespace hod::window;

namespace hod::input
{
	/// @brief
	/// @param errorMessage
	void GetWinLastErrorMsg(String& /*errorMessage*/)
	{
		// TODO
	}

	ApiRawInput* ApiRawInput::_pInstance = nullptr;
	HHOOK        ApiRawInput::_hGetMessageHook = nullptr;

	/// @brief
	/// @param code
	/// @param wParam
	/// @param lParam
	/// @return
	LRESULT CALLBACK ApiRawInput::GetMessageHook(int code, WPARAM wParam, LPARAM lParam)
	{
		if (code < 0)
		{
			return CallNextHookEx(NULL, code, wParam, lParam);
		}

		if (code == HC_ACTION && wParam == PM_REMOVE)
		{
			if (ApiRawInput::_pInstance != nullptr)
			{
				MSG* pMsg = (MSG*)lParam;

				ApiRawInput::_pInstance->ProcessWindowMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			}
		}

		return CallNextHookEx(_hGetMessageHook, code, wParam, lParam);
	}

	/// @brief
	ApiRawInput::ApiRawInput()
	: Api("RawInput")
	, _onFocusChangeSlot(std::bind(&ApiRawInput::OnFocusChange, this, std::placeholders::_1))
	{
	}

	/// @brief
	/// @return
	bool ApiRawInput::Initialize()
	{
		HWND hwnd = NULL; // TODO

		RAWINPUTDEVICE aRawInputDevices[2];

		aRawInputDevices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		aRawInputDevices[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		aRawInputDevices[0].dwFlags = /*RIDEV_INPUTSINK | */ RIDEV_DEVNOTIFY; // RIDEV_NOLEGACY adds HID mouse and also ignores legacy mouse messages
		aRawInputDevices[0].hwndTarget = hwnd;

		aRawInputDevices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
		aRawInputDevices[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
		aRawInputDevices[1].dwFlags = /*RIDEV_INPUTSINK | */ RIDEV_DEVNOTIFY; // RIDEV_NOLEGACY adds HID keyboard and also ignores legacy keyboard messages
		aRawInputDevices[1].hwndTarget = hwnd;

		if (RegisterRawInputDevices(aRawInputDevices, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
		{
			String sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to register RawInput devices type ({})", sErrorMessage);
			return false;
		}

		assert(ApiRawInput::_pInstance == nullptr);
		ApiRawInput::_pInstance = this;

		_window = static_cast<Win32Window*>(Win32DisplayManager::GetInstance()->GetMainWindow());

		assert(ApiRawInput::_hGetMessageHook == nullptr);
		ApiRawInput::_hGetMessageHook = SetWindowsHookEx(WH_GETMESSAGE, ApiRawInput::GetMessageHook, NULL, _window->GetMessageLoopThreadId());
		if (ApiRawInput::_hGetMessageHook == nullptr)
		{
			String sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to set MessageHook ({})", sErrorMessage);
		}

		_window->GetFocusedEvent().Connect(_onFocusChangeSlot);

		SetInitialized(true);

		FetchConnectedDevices();

		return true;
	}

	/// @brief
	void ApiRawInput::FetchConnectedDevices()
	{
		UINT                deviceCount = 0;
		PRAWINPUTDEVICELIST pRawInputDeviceList = nullptr;

		if (GetRawInputDeviceList(nullptr, &deviceCount, sizeof(RAWINPUTDEVICELIST)) != 0)
		{
			String sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput device list ({})", sErrorMessage);
			return;
		}

		pRawInputDeviceList = DefaultAllocator::GetInstance().NewArray<RAWINPUTDEVICELIST>(deviceCount);

		if (GetRawInputDeviceList(pRawInputDeviceList, &deviceCount, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			String sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput device list ({})", sErrorMessage);
			return;
		}

		for (uint32_t deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
		{
			const RAWINPUTDEVICELIST& device = pRawInputDeviceList[deviceIndex];

			PushDeviceChangeMessage(device.hDevice, GIDC_ARRIVAL);
		}

		DefaultAllocator::GetInstance().DeleteArray(pRawInputDeviceList);
	}

	/// @brief
	/// @param hDevice
	/// @param sName
	/// @param info
	/// @return
	Device* ApiRawInput::GetOrAddDevice(HANDLE hDevice, const std::string_view& name, const RID_DEVICE_INFO& info)
	{
		if (info.dwType == RIM_TYPEMOUSE)
		{
			MouseRawInput* mouse = FindMouse(hDevice);

			if (mouse == nullptr)
			{
				mouse = DefaultAllocator::GetInstance().New<MouseRawInput>(hDevice, name, info.mouse);

				_mice.push_back(mouse);

				AddDevice(mouse);
			}

			return mouse;
		}
		else if (info.dwType == RIM_TYPEKEYBOARD)
		{
			KeyboardRawInput* keyboard = FindKeyboard(hDevice);

			if (keyboard == nullptr)
			{
				keyboard = DefaultAllocator::GetInstance().New<KeyboardRawInput>(hDevice, name, info.keyboard);

				_keyboards.push_back(keyboard);

				AddDevice(keyboard);
			}

			return keyboard;
		}

		return nullptr;
	}

	/// @brief
	ApiRawInput::~ApiRawInput()
	{
		if (ApiRawInput::_hGetMessageHook != nullptr)
		{
			if (UnhookWindowsHookEx(ApiRawInput::_hGetMessageHook) == FALSE)
			{
				String sErrorMessage;
				GetWinLastErrorMsg(sErrorMessage);

				OUTPUT_ERROR("Unable to release MessageHook ({})", sErrorMessage);
			}
			else
			{
				ApiRawInput::_hGetMessageHook = nullptr;
			}
		}

		ApiRawInput::_pInstance = nullptr;

		for (MouseRawInput* mouse : _mice)
		{
			if (mouse->IsConnected() == true)
			{
				Api::NotifyDeviceDisconnected(mouse);
			}
			DefaultAllocator::GetInstance().Delete(mouse);
		}

		for (KeyboardRawInput* keyboard : _keyboards)
		{
			if (keyboard->IsConnected() == true)
			{
				Api::NotifyDeviceDisconnected(keyboard);
			}
			DefaultAllocator::GetInstance().Delete(keyboard);
		}
	}

	/// @brief
	/// @param uiMsg
	/// @param wParam
	/// @param lParam
	void ApiRawInput::ProcessWindowMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uiMsg == WM_INPUT_DEVICE_CHANGE)
		{
			PushDeviceChangeMessage((HANDLE)lParam, (uint8_t)wParam);
		}
		else if (uiMsg == WM_INPUT)
		{
			PushRawInputMessage((HRAWINPUT)lParam);
		}
		else if (uiMsg == WM_CHAR)
		{
			PushCharacterMessage((char)wParam);
		}
	}

	/// @brief
	/// @param bFocus
	void ApiRawInput::OnFocusChange(bool bFocus)
	{
		if (bFocus == false)
		{
		}
		else
		{
			_bJustGainFocus = true;
		}
	}

	/// @brief
	/// @param hDevice
	/// @param uiChangeFlag
	void ApiRawInput::PushDeviceChangeMessage(HANDLE hDevice, uint8_t changeFlag)
	{
		DeviceChangeMessage deviceChangeMessage;
		deviceChangeMessage._hDevice = hDevice;
		deviceChangeMessage._changeFlag = changeFlag;

		if (changeFlag == GIDC_ARRIVAL)
		{
			uint32_t uiSize = 512;
			if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, deviceChangeMessage._name, &uiSize) <= 0)
			{
				String sErrorMessage;
				GetWinLastErrorMsg(sErrorMessage);

				OUTPUT_ERROR("Unable to get RawInput device info (device handle = {}) ({})", (void*)hDevice, sErrorMessage);
				return;
			}

			uiSize = sizeof(RID_DEVICE_INFO);
			memset(&deviceChangeMessage._info, 0, uiSize);
			deviceChangeMessage._info.cbSize = uiSize;

			if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &deviceChangeMessage._info, &uiSize) <= 0)
			{
				String sErrorMessage;
				GetWinLastErrorMsg(sErrorMessage);

				OUTPUT_ERROR("Unable to get RawInput device info (device handle = {}) ({})", (void*)hDevice, sErrorMessage);
				return;
			}
		}

		_deviceChangeslock.lock();
		_vDeviceChangeMessages.push_back(deviceChangeMessage);
		_deviceChangeslock.unlock();
	}

	/// @brief
	/// @param hRawInput
	void ApiRawInput::PushRawInputMessage(HRAWINPUT hRawInput)
	{
		UINT uiSize = 0;
		if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &uiSize, sizeof(RAWINPUTHEADER)) != 0)
		{
			String sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput data (handle = {}) ({})", (void*)hRawInput, sErrorMessage);
			return;
		}

		if (uiSize == 0)
		{
			return;
		}

		RAWINPUT rawInputData;
		if (GetRawInputData(hRawInput, RID_INPUT, reinterpret_cast<BYTE*>(&rawInputData), &uiSize, sizeof(RAWINPUTHEADER)) != uiSize)
		{
			String sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput data (handle = {}) ({})", (void*)hRawInput, sErrorMessage);
			return;
		}

		HANDLE hDevice = rawInputData.header.hDevice;

		if (rawInputData.header.dwType == RIM_TYPEMOUSE)
		{
			MouseRawInput* mouse = FindMouse(hDevice);

			if (mouse != nullptr)
			{
				mouse->ReadRawInput(rawInputData.data.mouse);
			}
		}
		else if (rawInputData.header.dwType == RIM_TYPEKEYBOARD)
		{
			KeyboardRawInput* keyboard = FindKeyboard(hDevice);

			if (keyboard != nullptr)
			{
				keyboard->ReadRawInput(rawInputData.data.keyboard);
			}
		}
	}

	/// @brief
	/// @param cCharacter
	void ApiRawInput::PushCharacterMessage(char cCharacter)
	{
		_characterLock.lock();
		_vCharacterMessages.push_back(cCharacter);
		_characterLock.unlock();
	}

	/// @brief
	void ApiRawInput::UpdateDeviceValues()
	{
		if (_bJustGainFocus == true)
		{
			_bJustGainFocus = false;

			for (MouseRawInput* mouse : _mice)
			{
				mouse->ResyncLastCusorPosition();
			}
		}

		for (KeyboardRawInput* keyboard : _keyboards)
		{
			keyboard->ClearBufferedTextIfNeeded();
		}

		PullDeviceChangeMessages();
		PullCharacterMessages();

		for (MouseRawInput* mouse : _mice)
		{
			mouse->UpdateState();
		}

		for (KeyboardRawInput* keyboard : _keyboards)
		{
			keyboard->UpdateState();
		}
	}

	/// @brief
	void ApiRawInput::PullDeviceChangeMessages()
	{
		_deviceChangeslock.lock();

		for (DeviceChangeMessage& deviceChangeMessage : _vDeviceChangeMessages)
		{
			Device* device = GetOrAddDevice(deviceChangeMessage._hDevice, deviceChangeMessage._name, deviceChangeMessage._info);
			if (device != nullptr)
			{
				if (deviceChangeMessage._changeFlag == GIDC_ARRIVAL)
				{
					Api::SetDeviceConnected(device, true);
					Api::NotifyDeviceConnected(device);
				}
				else if (deviceChangeMessage._changeFlag == GIDC_REMOVAL)
				{
					Api::SetDeviceConnected(device, false);
					Api::NotifyDeviceDisconnected(device);
				}
			}
		}

		_vDeviceChangeMessages.Clear();

		_deviceChangeslock.unlock();
	}

	/// @brief
	void ApiRawInput::PullCharacterMessages()
	{
		_characterLock.lock();

		if (_vCharacterMessages.Size() != 0)
		{
			_vCharacterMessages.push_back('\0');

			if (_keyboards.Size() > 0)
			{
				// _keyboards[0]->AppendCharactersToBufferedText(_characterMessages.Data()); // TODO
			}

			_vCharacterMessages.Clear();
		}

		_characterLock.unlock();
	}

	/// @brief
	/// @param hDevice
	/// @return
	MouseRawInput* ApiRawInput::FindMouse(HANDLE hDevice) const
	{
		for (MouseRawInput* mouse : _mice)
		{
			if (mouse->GetHandle() == hDevice)
			{
				return mouse;
			}
		}

#if defined(REDIRECT_INJECTED_INPUT_TO_FIRST_DEVICE)
		if (_mice.Size() != 0)
		{
			return _mice[0];
		}
#endif

		return nullptr;
	}

	/// @brief
	/// @param hDevice
	/// @return
	KeyboardRawInput* ApiRawInput::FindKeyboard(HANDLE hDevice) const
	{
		for (KeyboardRawInput* keyboard : _keyboards)
		{
			if (keyboard->GetHandle() == hDevice)
			{
				return keyboard;
			}
		}

#if defined(REDIRECT_INJECTED_INPUT_TO_FIRST_DEVICE)
		if (_keyboards.Size() != 0)
		{
			return _keyboards[0];
		}
#endif

		return nullptr;
	}
}
