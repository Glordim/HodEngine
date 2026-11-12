#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline game
{
	/// @brief
	/// @param requiredComponent
	ReflectionTraitRequireComponent::ReflectionTraitRequireComponent(DescriptorGetter requiredComponent)
	: ReflectionTrait()
	, _requiredComponent(requiredComponent)
	{
	}

	/// @brief
	/// @return
	const ReflectionDescriptor& ReflectionTraitRequireComponent::GetRequiredComponent() const
	{
		return _requiredComponent();
	}

	/// @brief
	/// @param componentDescriptor
	/// @return
	Vector<const ReflectionDescriptor*> ReflectionTraitRequireComponent::GetRequiredComponents(const ReflectionDescriptor& componentDescriptor)
	{
		Vector<const ReflectionDescriptor*> requiredComponents;
		for (const ReflectionDescriptor* descriptor = &componentDescriptor; descriptor != nullptr; descriptor = descriptor->GetParent())
		{
			for (const ReflectionTrait* trait : descriptor->GetTraits())
			{
				if (trait->GetRttiType() != ReflectionTraitRequireComponent::GetRttiTypeStatic())
				{
					continue;
				}

				// Compared by type: each DLL has its own descriptor instance in shared builds
				const ReflectionDescriptor& requiredComponent = static_cast<const ReflectionTraitRequireComponent*>(trait)->GetRequiredComponent();
				bool                        alreadyListed = false;
				for (const ReflectionDescriptor* listed : requiredComponents)
				{
					if (listed->GetType() == requiredComponent.GetType())
					{
						alreadyListed = true;
						break;
					}
				}
				if (alreadyListed == false)
				{
					requiredComponents.PushBack(&requiredComponent);
				}
			}
		}
		return requiredComponents;
	}
}
