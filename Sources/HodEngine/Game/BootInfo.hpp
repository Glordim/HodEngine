#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/UID.hpp>

namespace hod::game
{
	class HOD_GAME_API BootInfo
	{
		REFLECTED_CLASS_NO_VIRTUAL(BootInfo)

	public:

		UID _startupScene;
		String _gameModule;
	};
}
