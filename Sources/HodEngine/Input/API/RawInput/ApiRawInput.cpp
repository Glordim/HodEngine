#include "HodEngine/Input/API/RawInput/ApiRawInput.hpp"

#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/API/RawInput/DeviceMouseRawInput.hpp"
#include "HodEngine/Input/API/RawInput/DeviceKeyboardRawInput.hpp"

#include <WinDNS.h>
#include <WinUser.h>
#include <hidusage.h>

#include <HodEngine/Core/Output.hpp>
#include <HodEngine/Application/GraphicApplications/DesktopApplications/WindowsApplication.hpp>
#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp>

using namespace hod::application;
using namespace hod::window;

namespace hod::input
{
	/// @brief 
	/// @param errorMessage 
	void GetWinLastErrorMsg(std::string& errorMessage)
	{
		// TODO
	}

	ApiRawInput* ApiRawInput::_pInstance = nullptr;
	HHOOK ApiRawInput::_hGetMessageHook = nullptr;

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
		aRawInputDevices[0].dwFlags = /*RIDEV_INPUTSINK | */RIDEV_DEVNOTIFY; // RIDEV_NOLEGACY adds HID mouse and also ignores legacy mouse messages
		aRawInputDevices[0].hwndTarget = hwnd;

		aRawInputDevices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
		aRawInputDevices[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
		aRawInputDevices[1].dwFlags = /*RIDEV_INPUTSINK | */RIDEV_DEVNOTIFY; // RIDEV_NOLEGACY adds HID keyboard and also ignores legacy keyboard messages
		aRawInputDevices[1].hwndTarget = hwnd;

		if (RegisterRawInputDevices(aRawInputDevices, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
		{
			std::string sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to register RawInput devices type ({})", sErrorMessage.c_str());
			return false;
		}

		assert(ApiRawInput::_pInstance == nullptr);
		ApiRawInput::_pInstance = this;

		Win32Window* window = static_cast<Win32Window*>(WindowsApplication::GetInstance()->GetWindow());

		assert(ApiRawInput::_hGetMessageHook == nullptr);
		ApiRawInput::_hGetMessageHook = SetWindowsHookEx(WH_GETMESSAGE, ApiRawInput::GetMessageHook, NULL, window->GetMessageLoopThreadId());
		if (ApiRawInput::_hGetMessageHook == nullptr)
		{
			std::string sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to set MessageHook ({})", sErrorMessage.c_str());
		}

		window->GetFocusedEvent().Connect(_onFocusChangeSlot);

		SetInitialized(true);

		FetchConnectedDevices();

		return true;
	}

	/// @brief 
	void ApiRawInput::FetchConnectedDevices()
	{
		UINT deviceCount = 0;
		PRAWINPUTDEVICELIST pRawInputDeviceList = nullptr;

		if (GetRawInputDeviceList(nullptr, &deviceCount, sizeof(RAWINPUTDEVICELIST)) != 0)
		{
			std::string sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput device list ({})", sErrorMessage.c_str());
			return;
		}

		pRawInputDeviceList = new RAWINPUTDEVICELIST[deviceCount];

		if (GetRawInputDeviceList(pRawInputDeviceList, &deviceCount, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			std::string sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput device list ({})", sErrorMessage.c_str());
			return;
		}

		for (uint32_t deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
		{
			const RAWINPUTDEVICELIST& device = pRawInputDeviceList[deviceIndex];

			PushDeviceChangeMessage(device.hDevice, GIDC_ARRIVAL);
		}

		delete[] pRawInputDeviceList;
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
			DeviceMouseRawInput* mouse = FindMouse(hDevice);

			if (mouse == nullptr)
			{
				mouse = new DeviceMouseRawInput(hDevice, name, info.mouse);

				_mice.push_back(mouse);

				AddDevice(mouse);
			}

			return mouse;
		}
		else if (info.dwType == RIM_TYPEKEYBOARD)
		{
			DeviceKeyboardRawInput* keyboard = FindKeyboard(hDevice);

			if (keyboard == nullptr)
			{
				keyboard = new DeviceKeyboardRawInput(hDevice, name, info.keyboard);

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
				std::string sErrorMessage;
				GetWinLastErrorMsg(sErrorMessage);

				OUTPUT_ERROR("Unable to release MessageHook ({})", sErrorMessage.c_str());
			}
			else
			{
				ApiRawInput::_hGetMessageHook = nullptr;
			}
		}

		ApiRawInput::_pInstance = nullptr;

		for (DeviceMouseRawInput* mouse : _mice)
		{
			if (mouse->IsConnected() == true)
			{
				Api::NotifyDeviceDisconnected(mouse);
			}
			delete mouse;
		}

		for (DeviceKeyboardRawInput* keyboard : _keyboards)
		{
			if (keyboard->IsConnected() == true)
			{
				Api::NotifyDeviceDisconnected(keyboard);
			}
			delete keyboard;
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
			_inputChangesLock.lock();
			_vInputChangeMessages.clear();
			_inputChangesLock.unlock();
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
				std::string sErrorMessage;
				GetWinLastErrorMsg(sErrorMessage);

				OUTPUT_ERROR("Unable to get RawInput device info (device handle = {}) ({})", hDevice, sErrorMessage.c_str());
				return;
			}

			uiSize = sizeof(RID_DEVICE_INFO);
			memset(&deviceChangeMessage._info, 0, uiSize);
			deviceChangeMessage._info.cbSize = uiSize;

			if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &deviceChangeMessage._info, &uiSize) <= 0)
			{
				std::string sErrorMessage;
				GetWinLastErrorMsg(sErrorMessage);

				OUTPUT_ERROR("Unable to get RawInput device info (device handle = {}) ({})", hDevice, sErrorMessage.c_str());
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
			std::string sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput data (handle = {}) ({})", hRawInput, sErrorMessage.c_str());
			return;
		}

		if (uiSize == 0)
		{
			return;
		}

		InputChangeMessage inputChangeMessage;
		if (GetRawInputData(hRawInput, RID_INPUT, reinterpret_cast<BYTE*>(&inputChangeMessage._rawInput), &uiSize, sizeof(RAWINPUTHEADER)) != uiSize)
		{
			std::string sErrorMessage;
			GetWinLastErrorMsg(sErrorMessage);

			OUTPUT_ERROR("Unable to get RawInput data (handle = {}) ({})", hRawInput, sErrorMessage.c_str());
			return;
		}

		_inputChangesLock.lock();
		_vInputChangeMessages.push_back(inputChangeMessage);
		_inputChangesLock.unlock();
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

			for (DeviceMouseRawInput* mouse : _mice)
			{
				mouse->ResyncLastCusorPosition();
			}
		}

		for (DeviceMouseRawInput* mouse : _mice)
		{
			mouse->PrepareUpdate();
		}

		for (DeviceKeyboardRawInput* keyboard : _keyboards)
		{
			keyboard->ClearBufferedTextIfNeeded();
		}

		PullDeviceChangeMessages();
		PullRawInputMessages();
		PullCharacterMessages();
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

		_vDeviceChangeMessages.clear();

		_deviceChangeslock.unlock();
	}

	/// @brief 
	void ApiRawInput::PullRawInputMessages()
	{
		_inputChangesLock.lock();
		if (_vInputChangeMessages.empty() == false)
		{
			Win32Window* window = static_cast<Win32Window*>(WindowsApplication::GetInstance()->GetWindow());
			if (window->IsFocused() == true)
			{
				bool isResizingOrMovingWindow = false; // TODO WindowsApplication::GetInstance()->GetIsResizingOrMoving();
				if (isResizingOrMovingWindow == true)
				{
					_bIgnoreNextMouseMessage = true; // Windows do the sum of all movement at the end of the Resizing or Moving
				}

				for (const InputChangeMessage& inputChangeMessage : _vInputChangeMessages)
				{
					HANDLE hDevice = inputChangeMessage._rawInput.header.hDevice;

					if (inputChangeMessage._rawInput.header.dwType == RIM_TYPEMOUSE)
					{
						DeviceMouseRawInput* mouse = FindMouse(hDevice);

						if (mouse != nullptr && isResizingOrMovingWindow == false)
						{
							if (_bIgnoreNextMouseMessage == true)
							{
								_bIgnoreNextMouseMessage = false;
							}
							else
							{
								mouse->ReadRawInput(inputChangeMessage._rawInput.data.mouse);
							}
						}
					}
					else if (inputChangeMessage._rawInput.header.dwType == RIM_TYPEKEYBOARD)
					{
						DeviceKeyboardRawInput* keyboard = FindKeyboard(hDevice);

						if (keyboard != nullptr)
						{
							keyboard->ReadRawInput(inputChangeMessage._rawInput.data.keyboard);
						}
					}
				}
			}
			_vInputChangeMessages.clear();
		}

		_inputChangesLock.unlock();
	}

	/// @brief 
	void ApiRawInput::PullCharacterMessages()
	{
		_characterLock.lock();

		if (_vCharacterMessages.size() != 0)
		{
			_vCharacterMessages.push_back('\0');

			if (_keyboards.size() > 0)
			{
				// _keyboards[0]->AppendCharactersToBufferedText(_characterMessages.data()); // TODO
			}

			_vCharacterMessages.clear();
		}

		_characterLock.unlock();
	}

	/// @brief 
	/// @param hDevice 
	/// @return 
	DeviceMouseRawInput* ApiRawInput::FindMouse(HANDLE hDevice) const
	{
		for (DeviceMouseRawInput* mouse : _mice)
		{
			if (mouse->GetHandle() == hDevice)
			{
				return mouse;
			}
		}

#if defined(REDIRECT_INJECTED_INPUT_TO_FIRST_DEVICE)
		if (_mice.size() != 0)
		{
			return _mice[0];
		}
#endif

		return nullptr;
	}

	/// @brief 
	/// @param hDevice 
	/// @return 
	DeviceKeyboardRawInput* ApiRawInput::FindKeyboard(HANDLE hDevice) const
	{
		for (DeviceKeyboardRawInput* keyboard : _keyboards)
		{
			if (keyboard->GetHandle() == hDevice)
			{
				return keyboard;
			}
		}

#if defined (REDIRECT_INJECTED_INPUT_TO_FIRST_DEVICE)
		if (_keyboards.size() != 0)
		{
			return _keyboards[0];
		}
#endif

		return nullptr;
	}
}
