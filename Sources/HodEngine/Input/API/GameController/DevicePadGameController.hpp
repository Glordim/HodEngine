#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/DevicePad.hpp"
#include "HodEngine/Input/InputButton.hpp"
#include "HodEngine/Input/State.hpp"

#ifdef __OBJC__
@class GCExtendedGamepad;
#else
class GCExtendedGamepad;
#endif

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API DevicePadGameController : public DevicePad
	{
	public:
											DevicePadGameController(GCExtendedGamepad* extendedGamepad);
											DevicePadGameController(const DevicePadGameController&) = delete;
											DevicePadGameController(DevicePadGameController&&) = delete;
											~DevicePadGameController() override = default;

		DevicePadGameController&			operator = (const DevicePadGameController&) = delete;
		DevicePadGameController&			operator = (DevicePadGameController&&) = delete;

		GCExtendedGamepad*					GetInternalExtendedPad() const;

		void								WriteNextState();

	protected:

		bool								ApplyFeedback(Feedback& feedback) override;

	private:

		GCExtendedGamepad*					_extendedGamepad;
	};
}
