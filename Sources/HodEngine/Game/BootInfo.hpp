#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/UID.hpp>

namespace hod::game
{
	class HOD_GAME_API BootInfo
	{
		REFLECTED_CLASS_NO_VIRTUAL(BootInfo, HOD_GAME_API)

	public:

		UID _startupScene;
		std::string _gameModule;
	};
}
