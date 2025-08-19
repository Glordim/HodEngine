#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Core/Event.hpp"

#include <mutex>

#ifdef __OBJC__
@class GCExtendedGamepad;
#else
class GCExtendedGamepad;
#endif

namespace hod::input
{
	class DevicePadGameController;

	/// @brief 
	class HOD_INPUT_API ApiGameController : public Api
	{
	public:

											ApiGameController();
											ApiGameController(const ApiGameController&) = delete;
											ApiGameController(ApiGameController&&) = delete;
											~ApiGameController() override;

		ApiGameController&					operator=(const ApiGameController&) = delete;
		ApiGameController&					operator=(ApiGameController&&) = delete;

	public:

		bool								Initialize() override;

	protected:

		void								UpdateDeviceValues() override;

	private:

		void								AddPadDevice(GCExtendedGamepad* extendedGamepad);
		void								RemovePadDevice(GCExtendedGamepad* extendedGamepad);

	private:

		Vector<DevicePadGameController*>	_pads;

		void*								_connectObserver = nullptr;
		void*								_disconnectObserver = nullptr;
	};
}
