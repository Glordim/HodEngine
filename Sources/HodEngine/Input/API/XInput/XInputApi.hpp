#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"

struct _XINPUT_STATE;
struct _XINPUT_VIBRATION;
struct HINSTANCE__;
using DWORD = unsigned long;

#define XUSER_MAX_COUNT 4

namespace hod::inline input
{
	class XInputGamepad;

	/// @brief
	class HOD_INPUT_API XInputApi : public Api
	{
	private:
		friend class XInputGamepad;

		using XInputGetStateProc = DWORD (*)(DWORD userIndex, _XINPUT_STATE* state);
		using XInputSetStateProc = DWORD (*)(DWORD userIndex, _XINPUT_VIBRATION* vibration);

		static constexpr uint32_t MaxPad = XUSER_MAX_COUNT;

	public:
		XInputApi();
		XInputApi(const XInputApi&) = delete;
		XInputApi(XInputApi&&) = delete;
		~XInputApi() override;

		XInputApi& operator=(const XInputApi&) = delete;
		XInputApi& operator=(XInputApi&&) = delete;

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

		XInputGamepad* _pads[MaxPad] = {nullptr};
	};
}
