#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/Components/RendererComponent.hpp"

#include <HodEngine/Game/Components/Node2dComponent.hpp>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_CLASS(RendererComponent)
	{
		HOD_REQUIRED_COMPONENT(_node2d);
	}
}
