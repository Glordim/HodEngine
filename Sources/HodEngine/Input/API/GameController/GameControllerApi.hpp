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

namespace hod::inline input
{
	class GameControllerGamepad;

	/// @brief 
	class HOD_INPUT_API GameControllerApi : public Api
	{
	public:

											GameControllerApi();
											GameControllerApi(const GameControllerApi&) = delete;
											GameControllerApi(GameControllerApi&&) = delete;
											~GameControllerApi() override;

		GameControllerApi&					operator=(const GameControllerApi&) = delete;
		GameControllerApi&					operator=(GameControllerApi&&) = delete;

	public:

		bool								Initialize() override;

	protected:

		void								UpdateDeviceValues() override;

	private:

		void								AddPadDevice(GCExtendedGamepad* extendedGamepad);
		void								RemovePadDevice(GCExtendedGamepad* extendedGamepad);

	private:

		Vector<GameControllerGamepad*>		_pads;

		void*								_connectObserver = nullptr;
		void*								_disconnectObserver = nullptr;
	};
}
