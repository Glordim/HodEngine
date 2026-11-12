#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/Components/RendererComponent.hpp"

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_CLASS(RendererComponent, reflectionDescriptor)
	{
		// Drawn where its node is
		reflectionDescriptor.AddTrait<ReflectionTraitRequireComponent>(&Node2dComponent::GetReflectionDescriptor);
	}
}
