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
	/// @brief 
	/// @param entity 
	/// @param diffs 
	/// @return 
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
					ReflectionDescriptor* reflectionDescriptor = componentLock->GetReflectionDescriptorV();
					const void* componentAddr = componentLock.get();
					const void* instanceAddr = instanceComponentLock.get();
					CollectDiff(componentLock, instanceComponentLock, diffs, reflectionDescriptor, componentAddr, instanceAddr);
				}
			}
		}

		return true;
	}

	/// @brief 
	/// @param sourceComponent 
	/// @param instanceComponent 
	/// @param diffs 
	/// @param reflectionDescriptor 
	/// @param sourceAddr 
	/// @param instanceAddr 
	void CollectDiff(std::shared_ptr<Component> sourceComponent, std::shared_ptr<Component> instanceComponent, EntityDiffs& diffs, ReflectionDescriptor* reflectionDescriptor, const void* sourceAddr, const void* instanceAddr)
	{
		for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
		{
			if (reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
			{
				ReflectionPropertyVariable* reflectionPropertyVariable = static_cast<ReflectionPropertyVariable*>(reflectionProperty);
				if (reflectionPropertyVariable->CompareInstance(sourceAddr, instanceAddr) == false)
				{
					diffs.Add(reflectionPropertyVariable, sourceComponent, instanceComponent);
				}
			}
			else if (reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
			{
				ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(reflectionProperty);
				const void* subObjectSourceAddr = reflectionPropertyObject->GetInstance(sourceAddr);
				const void* subObjectInstanceAddr = reflectionPropertyObject->GetInstance(instanceAddr);
				ReflectionDescriptor* subObjectReflectionDescriptor = reflectionPropertyObject->GetReflectionDescriptor();
				CollectDiff(sourceComponent, instanceComponent, diffs, subObjectReflectionDescriptor, subObjectSourceAddr, subObjectInstanceAddr);
			}
		}
	}
}
