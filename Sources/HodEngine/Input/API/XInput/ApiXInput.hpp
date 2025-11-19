#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"

struct _XINPUT_STATE;
struct _XINPUT_VIBRATION;
struct HINSTANCE__;
using DWORD = unsigned long;

#define XUSER_MAX_COUNT 4

namespace hod::input
{
	class GamepadXInput;

	/// @brief
	class HOD_INPUT_API ApiXInput : public Api
	{
	private:
		friend class GamepadXInput;

		using XInputGetStateProc = DWORD (*)(DWORD userIndex, _XINPUT_STATE* state);
		using XInputSetStateProc = DWORD (*)(DWORD userIndex, _XINPUT_VIBRATION* vibration);

		static constexpr uint32_t MaxPad = XUSER_MAX_COUNT;

	public:
		ApiXInput();
		ApiXInput(const ApiXInput&) = delete;
		ApiXInput(ApiXInput&&) = delete;
		~ApiXInput() override;

		ApiXInput& operator=(const ApiXInput&) = delete;
		ApiXInput& operator=(ApiXInput&&) = delete;

	public:
		bool Initialize() override;

	protected:
		void UpdateDeviceValues() override;

	private:
		bool GetPadState(uint32_t padIndex, _XINPUT_STATE* state) const;
		bool SetPadState(uint32_t padIndex, _XINPUT_VIBRATION* vibration) const;

	private:
		HINSTANCE__*       _hInstance = nullptr;
		XInputGetStateProc _getStateProc = nullptr;
		XInputSetStateProc _setStateProc = nullptr;

		GamepadXInput* _pads[MaxPad] = {nullptr};
	};
}
