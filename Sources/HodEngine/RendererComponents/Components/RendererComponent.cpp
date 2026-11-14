#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/Components/RendererComponent.hpp"

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_CLASS(RendererComponent)
	{
		// Drawn where its node is
		HOD_TRAIT(ReflectionTraitRequireComponent, &Node2dComponent::GetReflectionDescriptor);
	}
}
