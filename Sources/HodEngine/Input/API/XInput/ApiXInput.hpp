#pragma once
#include "HodEngine/Input/Export.hpp"

#include <Windows.h>
#include <Xinput.h>

#include "HodEngine/Input/Api.hpp"

namespace hod::input
{
	class DevicePadXbox;

	/// @brief 
	class HOD_INPUT_API ApiXInput : public Api
	{
	private:

		friend class DevicePadXbox;

		using XInputGetStateProc = DWORD(*)(DWORD userIndex, XINPUT_STATE* state);
		using XInputSetStateProc = DWORD(*)(DWORD userIndex, XINPUT_VIBRATION* vibration);

		static constexpr uint32_t MaxPad = XUSER_MAX_COUNT;

	public:
								ApiXInput();
								ApiXInput(const ApiXInput&) = delete;
								ApiXInput(ApiXInput&&) = delete;
								~ApiXInput() override;

		ApiXInput&				operator = (const ApiXInput&) = delete;
		ApiXInput&				operator = (ApiXInput&&) = delete;

	public:

		bool					Initialize(window::Window* window) override;

	protected:

		void					UpdateDeviceValues() override;

	private:

		bool					GetPadState(uint32_t padIndex, XINPUT_STATE* state) const;
		bool					SetPadState(uint32_t padIndex, XINPUT_VIBRATION* vibration) const;

	private:

		HINSTANCE				_hInstance = nullptr;
		XInputGetStateProc		_getStateProc = nullptr;
		XInputSetStateProc		_setStateProc = nullptr;

		DevicePadXbox*			_pads[MaxPad] = { nullptr };
	};
}
