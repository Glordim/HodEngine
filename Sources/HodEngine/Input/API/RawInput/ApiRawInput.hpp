#pragma once
#include "HodEngine/Input/Export.hpp"

#include <Windows.h>

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
	class DeviceMouseRawInput;
	class DeviceKeyboardRawInput;

	/// @brief 
	class HOD_INPUT_API ApiRawInput : public Api
	{
	public:

											ApiRawInput();
											ApiRawInput(const ApiRawInput&) = delete;
											ApiRawInput(ApiRawInput&&) = delete;
											~ApiRawInput() override;

		ApiRawInput&						operator=(const ApiRawInput&) = delete;
		ApiRawInput&						operator=(ApiRawInput&&) = delete;

	public:

		bool								Initialize(window::Window* window) override;

	protected:

		void								UpdateDeviceValues() override;

	private:

		struct DeviceChangeMessage
		{
			HANDLE				_hDevice = nullptr;
			uint8_t				_changeFlag = 0;
			char				_name[512] = { '\0' };
			RID_DEVICE_INFO		_info;
		};

		struct InputChangeMessage
		{
			RAWINPUT			_rawInput;
		};

	private:

		static LRESULT CALLBACK						GetMessageHook(int nCode, WPARAM wParam, LPARAM lParam);

	private:

		void										OnFocusChange(bool bFocus);

		void										FetchConnectedDevices();
		Device*										GetOrAddDevice(HANDLE hDevice, const std::string_view& name, const RID_DEVICE_INFO& info);

		void										ProcessWindowMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam);
		void										PushDeviceChangeMessage(HANDLE hDevice, uint8_t changeFlag);
		void										PushRawInputMessage(HRAWINPUT hRawInput);
		void										PushCharacterMessage(char character);

		void										PullDeviceChangeMessages();
		void										PullRawInputMessages();
		void										PullCharacterMessages();

		DeviceMouseRawInput*						FindMouse(HANDLE hDevice) const;
		DeviceKeyboardRawInput*						FindKeyboard(HANDLE hDevice) const;

	private:

		static ApiRawInput*							_pInstance;
		static HHOOK								_hGetMessageHook;

	private:

		std::vector<DeviceMouseRawInput*>			_mice;
		std::vector<DeviceKeyboardRawInput*>		_keyboards;

		std::mutex									_deviceChangeslock;
		std::vector<DeviceChangeMessage>			_vDeviceChangeMessages;

		std::mutex									_inputChangesLock;
		std::vector<InputChangeMessage>				_vInputChangeMessages;

		std::mutex									_characterLock;
		std::vector<char>							_vCharacterMessages;

		bool										_bIgnoreNextMouseMessage = false;
		bool										_bJustGainFocus = false;

		Event<bool>::Slot							_onFocusChangeSlot;

		window::Win32Window*						_window = nullptr;
	};
}
