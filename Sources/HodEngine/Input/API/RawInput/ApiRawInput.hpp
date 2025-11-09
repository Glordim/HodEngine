#pragma once
#include "HodEngine/Input/Export.hpp"

#include <Windows.h>

using HANDLE = void*;
struct HHOOK__;
struct HRAWINPUT__;
struct tagRID_DEVICE_INFO;

#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/InputId.hpp"

#include "HodEngine/Core/Event.hpp"

#include <mutex>

#define REDIRECT_INJECTED_INPUT_TO_FIRST_DEVICE // Used to support TeamViewer and Parsec (they don't fill hDevice and need a redirection)

namespace hod::window
{
	class Win32Window;
}

namespace hod::input
{
	class MouseRawInput;
	class KeyboardRawInput;

	/// @brief
	class HOD_INPUT_API ApiRawInput : public Api
	{
	public:
		ApiRawInput();
		ApiRawInput(const ApiRawInput&) = delete;
		ApiRawInput(ApiRawInput&&) = delete;
		~ApiRawInput() override;

		ApiRawInput& operator=(const ApiRawInput&) = delete;
		ApiRawInput& operator=(ApiRawInput&&) = delete;

	public:
		bool Initialize() override;

	protected:
		void UpdateDeviceValues() override;

	private:
		struct DeviceChangeMessage
		{
			HANDLE          _hDevice = nullptr;
			uint8_t         _changeFlag = 0;
			char            _name[512] = {'\0'};
			RID_DEVICE_INFO _info;
		};

	private:
		static LRESULT CALLBACK GetMessageHook(int nCode, WPARAM wParam, LPARAM lParam);

	private:
		void OnFocusChange(bool bFocus);

		void    FetchConnectedDevices();
		Device* GetOrAddDevice(HANDLE hDevice, const std::string_view& name, const tagRID_DEVICE_INFO& info);

		void ProcessWindowMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam);
		void PushDeviceChangeMessage(HANDLE hDevice, uint8_t changeFlag);
		void PushRawInputMessage(HRAWINPUT__* hRawInput);
		void PushCharacterMessage(char character);

		void PullDeviceChangeMessages();
		void PullCharacterMessages();

		MouseRawInput*    FindMouse(HANDLE hDevice) const;
		KeyboardRawInput* FindKeyboard(HANDLE hDevice) const;

		void OnWinProc(HWND, UINT, WPARAM, LPARAM);

	private:
		Event<HWND, UINT, WPARAM, LPARAM>::Slot _onWinProcSlot;

		Vector<MouseRawInput*>    _mice;
		Vector<KeyboardRawInput*> _keyboards;

		std::mutex                  _deviceChangeslock;
		Vector<DeviceChangeMessage> _vDeviceChangeMessages;

		std::mutex   _characterLock;
		Vector<char> _vCharacterMessages;

		// bool										_bIgnoreNextMouseMessage = false;
		bool _bJustGainFocus = false;

		Event<bool>::Slot _onFocusChangeSlot;

		window::Win32Window* _window = nullptr;
	};
}
