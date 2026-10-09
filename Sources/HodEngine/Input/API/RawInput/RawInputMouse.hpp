#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Mouse.hpp"
#include "HodEngine/Input/Input.hpp"

using HANDLE = void*;
struct tagRID_DEVICE_INFO_MOUSE;
struct tagRAWMOUSE;

namespace hod::inline input
{
	/// @brief
	class HOD_INPUT_API RawInputMouse : public Mouse
	{
	public:
		RawInputMouse(HANDLE handle, const std::string_view& name, const tagRID_DEVICE_INFO_MOUSE& info);
		RawInputMouse(const RawInputMouse&) = delete;
		RawInputMouse(RawInputMouse&&) = delete;
		~RawInputMouse() override = default;

		RawInputMouse& operator=(const RawInputMouse&) = delete;
		RawInputMouse& operator=(RawInputMouse&&) = delete;

	public:
		HANDLE GetHandle() const;

		void ResyncLastCusorPosition();
		void SimulateMouseDownBeforeFocusGain(InputId mouseButtonInputId);
		void ReadRawInput(const tagRAWMOUSE& rawMouse);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;

		void ResetNextState() override;

	private:
		static UID ComputeDeviceUID(HANDLE hDevice);

	private:
		HANDLE _handle = nullptr;

		int32_t _lastAbsoluteX = 0;
		int32_t _lastAbsoluteY = 0;
		bool    _lastAbsoluteDirty = true;
	};
}
