#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/Prefab.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"

#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output.hpp"
#include "HodEngine/Core/Color.hpp"

namespace hod::game::PrefabUtility
{
	bool CollectDiff(std::shared_ptr<Entity> entity, EntityDiffs& diffs)
	{
		Prefab* prefab = entity->GetPrefab();
		if (prefab == nullptr)
		{
			return false;
		}

		std::shared_ptr<Entity> source = prefab->GetRootEntity();
		std::shared_ptr<Entity> instance = entity;

		ReflectionDescriptor* reflectionDescriptor = source->GetReflectionDescriptorV();
		for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
		{
			if (reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
			{
				ReflectionPropertyVariable* reflectionPropertyVariable = static_cast<ReflectionPropertyVariable*>(reflectionProperty);
				if (reflectionPropertyVariable->CompareInstance(source.get(), instance.get()) == false)
				{
					diffs.Add(reflectionPropertyVariable, source, instance);
				}
			}
		}

		for (std::weak_ptr<Component> component : source->GetComponents())
		{
			std::shared_ptr<Component> componentLock = component.lock();

			UID localId = componentLock->GetLocalId();
			for (std::weak_ptr<Component> instanceComponent : instance->GetComponents())
			{
				std::shared_ptr<Component> instanceComponentLock = instanceComponent.lock();
				if (instanceComponentLock->GetLocalId() == localId)
				{
					reflectionDescriptor = componentLock->GetReflectionDescriptorV();
					for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
					{
						if (reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
						{
							ReflectionPropertyVariable* reflectionPropertyVariable = static_cast<ReflectionPropertyVariable*>(reflectionProperty);
							if (reflectionPropertyVariable->CompareInstance(componentLock.get(), instanceComponentLock.get()) == false)
							{
								diffs.Add(reflectionPropertyVariable, componentLock, instanceComponentLock);
							}
						}
					}
				}
			}
		}

		return true;
	}
}
