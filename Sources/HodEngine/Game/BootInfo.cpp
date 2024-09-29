#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/BootInfo.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS_NO_PARENT(BootInfo)
	{
		ADD_PROPERTY(BootInfo, _startupScene);
	}
}
