#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/RawInput/RawInputApi.hpp"

#include "HodEngine/Input/API/RawInput/RawInputKeyboard.hpp"
#include "HodEngine/Input/API/RawInput/RawInputMouse.hpp"
#include "HodEngine/Input/InputManager.hpp"

#include <hidusage.h>
#include <WinDNS.h>
#include <WinUser.h>

#include <HodEngine/Core/OS.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp>
#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp>

using namespace hod::window;

namespace hod::inline input
{
	/// @brief
	RawInputApi::RawInputApi()
	: Api("RawInput")
	, _onWinProcSlot(std::bind(&RawInputApi::OnWinProc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))
	, _onFocusChangeSlot(std::bind(&RawInputApi::OnFocusChange, this, std::placeholders::_1))
	{
	}

	/// @brief
	/// @return
	bool RawInputApi::Initialize()
	{
		_window = static_cast<Win32Window*>(Win32DisplayManager::GetInstance()->GetMainWindow());

		HWND hwnd = _window->GetWindowHandle();

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
			OUTPUT_ERROR("Unable to register RawInput devices type ({})", OS::GetLastWin32ErrorMessage());
			return false;
		}

		//_window->OnWinProc.Connect(_onWinProcSlot);

		//_window->GetFocusedEvent().Connect(_onFocusChangeSlot);

		SetInitialized(true);

		FetchConnectedDevices();

		return true;
	}

	void RawInputApi::OnWinProc(HWND, UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		ProcessWindowMessage(uiMsg, wParam, lParam);
	}

	/// @brief
	void RawInputApi::FetchConnectedDevices()
	{
		UINT                deviceCount = 0;
		PRAWINPUTDEVICELIST pRawInputDeviceList = nullptr;

		if (GetRawInputDeviceList(nullptr, &deviceCount, sizeof(RAWINPUTDEVICELIST)) != 0)
		{
			OUTPUT_ERROR("Unable to get RawInput device list ({})", OS::GetLastWin32ErrorMessage());
			return;
		}

		pRawInputDeviceList = DefaultAllocator::GetInstance().NewArray<RAWINPUTDEVICELIST>(deviceCount);

		if (GetRawInputDeviceList(pRawInputDeviceList, &deviceCount, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			OUTPUT_ERROR("Unable to get RawInput device list ({})", OS::GetLastWin32ErrorMessage());
			return;
		}

		for (uint32_t deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
		{
			const RAWINPUTDEVICELIST& device = pRawInputDeviceList[deviceIndex];

			if (device.dwType == RIM_TYPEMOUSE)
			{
				RID_DEVICE_INFO_MOUSE info = {}; // TODO useless ?
				RawInputMouse*        mouse = DefaultAllocator::GetInstance().New<RawInputMouse>(device.hDevice, "Mouse", info);
				_mice.PushBack(mouse);
				AddDevice(mouse);
			}
			else if (device.dwType == RIM_TYPEKEYBOARD)
			{
				RID_DEVICE_INFO_KEYBOARD info = {}; // TODO useless ?
				RawInputKeyboard*        keyboard = DefaultAllocator::GetInstance().New<RawInputKeyboard>(device.hDevice, "Keyboard", info);
				_keyboards.PushBack(keyboard);
				AddDevice(keyboard);
			}
		}

		DefaultAllocator::GetInstance().DeleteArray(pRawInputDeviceList);
	}

	/// @brief
	/// @param hDevice
	/// @param sName
	/// @param info
	/// @return
	Device* RawInputApi::GetOrAddDevice(HANDLE hDevice, const std::string_view& name, const RID_DEVICE_INFO& info)
	{
		if (info.dwType == RIM_TYPEMOUSE)
		{
			RawInputMouse* mouse = FindMouse(hDevice);

			if (mouse == nullptr)
			{
				mouse = DefaultAllocator::GetInstance().New<RawInputMouse>(hDevice, name, info.mouse);

				_mice.push_back(mouse);

				AddDevice(mouse);
			}

			return mouse;
		}
		else if (info.dwType == RIM_TYPEKEYBOARD)
		{
			RawInputKeyboard* keyboard = FindKeyboard(hDevice);

			if (keyboard == nullptr)
			{
				keyboard = DefaultAllocator::GetInstance().New<RawInputKeyboard>(hDevice, name, info.keyboard);

				_keyboards.push_back(keyboard);

				AddDevice(keyboard);
			}

			return keyboard;
		}

		return nullptr;
	}

	/// @brief
	RawInputApi::~RawInputApi()
	{
		for (RawInputMouse* mouse : _mice)
		{
			if (mouse->IsConnected() == true)
			{
				Api::NotifyDeviceDisconnected(mouse);
			}
			DefaultAllocator::GetInstance().Delete(mouse);
		}

		for (RawInputKeyboard* keyboard : _keyboards)
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
	void RawInputApi::ProcessWindowMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uiMsg == WM_INPUT_DEVICE_CHANGE)
		{
			uint32_t uiSize = 512;
			HANDLE   handle = (HANDLE)lParam;

			RID_DEVICE_INFO deviceInfo = {};
			deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
			if (GetRawInputDeviceInfoW(handle, RIDI_DEVICEINFO, &deviceInfo, &uiSize) <= 0)
			{
				OUTPUT_ERROR("Unable to get RawInput device info (device handle = {}) ({})", (void*)handle, OS::GetLastWin32ErrorMessage());
				return;
			}

			if (wParam == GIDC_ARRIVAL)
			{
				if (deviceInfo.dwType == RIM_TYPEMOUSE)
				{
					RawInputMouse* mouse = DefaultAllocator::GetInstance().New<RawInputMouse>(handle, "Mouse", deviceInfo.mouse);
					_pendingArrivalMice.PushBack(mouse);
				}
				else if (deviceInfo.dwType == RIM_TYPEKEYBOARD)
				{
					RawInputKeyboard* keyboard = DefaultAllocator::GetInstance().New<RawInputKeyboard>(handle, "Keyboard", deviceInfo.keyboard);
					_pendingArrivalKeyboards.PushBack(keyboard);
				}
			}
			else if (wParam == GIDC_REMOVAL)
			{
				if (deviceInfo.dwType == RIM_TYPEMOUSE)
				{
					_pendingRemoveMice.PushBack(handle);
				}
				else if (deviceInfo.dwType == RIM_TYPEKEYBOARD)
				{
					_pendingRemoveKeyboards.PushBack(handle);
				}
			}
			else
			{
				assert(false);
			}
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
	void RawInputApi::OnFocusChange(bool bFocus)
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
	void RawInputApi::PushDeviceChangeMessage(HANDLE hDevice, uint8_t changeFlag)
	{
		DeviceChangeMessage deviceChangeMessage;
		deviceChangeMessage._hDevice = hDevice;
		deviceChangeMessage._changeFlag = changeFlag;

		if (changeFlag == GIDC_ARRIVAL)
		{
			uint32_t uiSize = 512;
			if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, deviceChangeMessage._name, &uiSize) <= 0)
			{
				OUTPUT_ERROR("Unable to get RawInput device info (device handle = {}) ({})", (void*)hDevice, OS::GetLastWin32ErrorMessage());
				return;
			}

			uiSize = sizeof(RID_DEVICE_INFO);
			memset(&deviceChangeMessage._info, 0, uiSize);
			deviceChangeMessage._info.cbSize = uiSize;

			if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &deviceChangeMessage._info, &uiSize) <= 0)
			{
				OUTPUT_ERROR("Unable to get RawInput device info (device handle = {}) ({})", (void*)hDevice, OS::GetLastWin32ErrorMessage());
				return;
			}
		}

		_deviceChangeslock.lock();
		_vDeviceChangeMessages.push_back(deviceChangeMessage);
		_deviceChangeslock.unlock();
	}

	/// @brief
	/// @param hRawInput
	void RawInputApi::PushRawInputMessage(HRAWINPUT hRawInput)
	{
		UINT uiSize = 0;
		if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &uiSize, sizeof(RAWINPUTHEADER)) != 0)
		{
			OUTPUT_ERROR("Unable to get RawInput data (handle = {}) ({})", (void*)hRawInput, OS::GetLastWin32ErrorMessage());
			return;
		}

		if (uiSize == 0)
		{
			return;
		}

		RAWINPUT rawInputData;
		if (GetRawInputData(hRawInput, RID_INPUT, reinterpret_cast<BYTE*>(&rawInputData), &uiSize, sizeof(RAWINPUTHEADER)) != uiSize)
		{
			OUTPUT_ERROR("Unable to get RawInput data (handle = {}) ({})", (void*)hRawInput, OS::GetLastWin32ErrorMessage());
			return;
		}

		HANDLE hDevice = rawInputData.header.hDevice;

		if (rawInputData.header.dwType == RIM_TYPEMOUSE)
		{
			RawInputMouse* mouse = FindMouse(hDevice);

			if (mouse != nullptr)
			{
				mouse->ReadRawInput(rawInputData.data.mouse);
			}
		}
		else if (rawInputData.header.dwType == RIM_TYPEKEYBOARD)
		{
			RawInputKeyboard* keyboard = FindKeyboard(hDevice);

			if (keyboard != nullptr)
			{
				keyboard->ReadRawInput(rawInputData.data.keyboard);
			}
		}
	}

	/// @brief
	/// @param cCharacter
	void RawInputApi::PushCharacterMessage(char cCharacter)
	{
		_characterLock.lock();
		_vCharacterMessages.push_back(cCharacter);
		_characterLock.unlock();
	}

	/// @brief
	void RawInputApi::UpdateDeviceValues()
	{
		if (_bJustGainFocus == true)
		{
			_bJustGainFocus = false;

			for (RawInputMouse* mouse : _mice)
			{
				mouse->ResyncLastCusorPosition();
			}
		}

		for (RawInputMouse* arrivalMouse : _pendingArrivalMice)
		{
			_mice.PushBack(arrivalMouse);
			AddDevice(arrivalMouse);
		}
		_pendingArrivalMice.Clear();

		for (RawInputKeyboard* arrivalKeyboard : _pendingArrivalKeyboards)
		{
			_keyboards.PushBack(arrivalKeyboard);
			AddDevice(arrivalKeyboard);
		}
		_pendingArrivalKeyboards.Clear();

		for (HANDLE removeMouseHandle : _pendingRemoveMice)
		{
			auto it = std::find_if(_mice.Begin(), _mice.End(), [removeMouseHandle](const RawInputMouse* mouse) { return mouse->GetHandle() == removeMouseHandle; });
			if (it != _mice.End())
			{
				_mice.Erase(it);
			}
		}
		_pendingRemoveMice.Clear();

		for (HANDLE removeKeyboardHandle : _pendingRemoveKeyboards)
		{
			auto it = std::find_if(_keyboards.Begin(), _keyboards.End(),
			                       [removeKeyboardHandle](const RawInputKeyboard* keyboard) { return keyboard->GetHandle() == removeKeyboardHandle; });
			if (it != _keyboards.End())
			{
				_keyboards.Erase(it);
			}
		}
		_pendingRemoveKeyboards.Clear();

		for (RawInputMouse* arrivalMouse : _pendingArrivalMice)
		{
			_mice.PushBack(arrivalMouse);
		}
		_pendingArrivalMice.Clear();

		for (RawInputKeyboard* keyboard : _keyboards)
		{
			keyboard->ClearBufferedTextIfNeeded();
		}

		PullDeviceChangeMessages();
		PullCharacterMessages();

		for (RawInputMouse* mouse : _mice)
		{
			mouse->UpdateState();
		}

		for (RawInputKeyboard* keyboard : _keyboards)
		{
			keyboard->UpdateState();
		}
	}

	/// @brief
	void RawInputApi::PullDeviceChangeMessages()
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
	void RawInputApi::PullCharacterMessages()
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
	RawInputMouse* RawInputApi::FindMouse(HANDLE hDevice) const
	{
		for (RawInputMouse* mouse : _mice)
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
	RawInputKeyboard* RawInputApi::FindKeyboard(HANDLE hDevice) const
	{
		for (RawInputKeyboard* keyboard : _keyboards)
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
