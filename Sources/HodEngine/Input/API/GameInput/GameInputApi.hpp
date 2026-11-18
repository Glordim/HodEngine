#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"

#include <HodEngine/Core/Concurency/SpinLock.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

namespace GameInput::v4
{
	struct IGameInput;
	struct IGameInputDevice;
}

namespace hod::inline input
{
	class GameInputGamepad;

	/// @brief The gamepads through GameInput (Windows 10 19H1+): its runtime is the most recent of the system's and of the
	/// GameInputRedist.dll deployed next to the executable. Initialize fails without one, InputManager falls back to XInput then.
	/// A gamepad is kept once seen (disconnected, then found again by its device id when it comes back), never deleted before the api
	class HOD_INPUT_API GameInputApi : public Api
	{
	public:
		GameInputApi();
		GameInputApi(const GameInputApi&) = delete;
		GameInputApi(GameInputApi&&) = delete;
		~GameInputApi() override;

		GameInputApi& operator=(const GameInputApi&) = delete;
		GameInputApi& operator=(GameInputApi&&) = delete;

	public:
		bool Initialize() override;

	protected:
		void UpdateDeviceValues() override;

	private:
		friend struct GameInputApiCallbacks;

		/// @brief A connection or a disconnection, given by GameInput on its own thread, applied by UpdateDeviceValues
		struct DeviceEvent
		{
			::GameInput::v4::IGameInputDevice* _device = nullptr; // Referenced until applied
			bool                               _connected = false;
		};

		void QueueDeviceEvent(::GameInput::v4::IGameInputDevice* device, bool connected);
		void ApplyDeviceEvents();

		GameInputGamepad* FindPad(::GameInput::v4::IGameInputDevice* device) const;

	private:
		::GameInput::v4::IGameInput* _gameInput = nullptr;
		uint64_t                     _deviceCallbackToken = 0;

		SpinLock            _deviceEventsLock;
		Vector<DeviceEvent> _deviceEvents;

		Vector<GameInputGamepad*> _pads;
	};
}
