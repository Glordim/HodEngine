#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp"

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline game
{
	/// @brief
	/// @param requiredComponent
	/// @param assign
	ReflectionTraitRequireComponent::ReflectionTraitRequireComponent(DescriptorGetter requiredComponent, AssignFunction assign)
	: ReflectionTrait()
	, _requiredComponent(requiredComponent)
	, _assign(std::move(assign))
	{
	}

	/// @brief
	/// @param requiring
	/// @return
	bool ReflectionTraitRequireComponent::Assign(Component& requiring) const
	{
		if (_assign == nullptr)
		{
			return true;
		}

		Component* required = requiring.GetComponent(GetRequiredComponent());
		_assign(requiring, required);
		if (required == nullptr)
		{
			OUTPUT_ERROR("{} requires {}, missing from its entity", requiring.GetReflectionDescriptorV().GetDisplayName(), GetRequiredComponent().GetDisplayName());
			return false;
		}
		return true;
	}

	/// @brief
	/// @param component
	void ReflectionTraitRequireComponent::AssignRequiredComponents(Component& component)
	{
		for (const ReflectionDescriptor* descriptor = &component.GetReflectionDescriptorV(); descriptor != nullptr; descriptor = descriptor->GetParent())
		{
			for (const ReflectionTrait* trait : descriptor->GetTraits())
			{
				if (trait->GetRttiType() == ReflectionTraitRequireComponent::GetRttiTypeStatic())
				{
					static_cast<const ReflectionTraitRequireComponent*>(trait)->Assign(component);
				}
			}
		}
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
