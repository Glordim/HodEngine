#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/BootInfo.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(BootInfo, reflectionDescriptor)
	{
		AddPropertyT(&reflectionDescriptor, &BootInfo::_startupScene, "_startupScene");
		AddPropertyT(&reflectionDescriptor, &BootInfo::_gameModule, "_gameModule");
	}
}
