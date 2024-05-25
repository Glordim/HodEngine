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
					diffs.Add("", reflectionPropertyVariable, source, instance, source.get(), instance.get()); // TODO uniformize with component, entity can have sub object too
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
					void* componentAddr = componentLock.get();
					void* instanceAddr = instanceComponentLock.get();
					std::string path = "";
					CollectDiff(componentLock, instanceComponentLock, diffs, path, reflectionDescriptor, componentAddr, instanceAddr);
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
	void CollectDiff(std::shared_ptr<Component> sourceComponent, std::shared_ptr<Component> instanceComponent, EntityDiffs& diffs, const std::string& path, ReflectionDescriptor* reflectionDescriptor, void* sourceAddr, void* instanceAddr)
	{
		if (reflectionDescriptor->GetParent() != nullptr)
		{
			CollectDiff(sourceComponent, instanceComponent, diffs, path, reflectionDescriptor->GetParent(), sourceAddr, instanceAddr);
		}

		for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
		{
			if (reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
			{
				ReflectionPropertyVariable* reflectionPropertyVariable = static_cast<ReflectionPropertyVariable*>(reflectionProperty);
				if (reflectionPropertyVariable->CompareInstance(sourceAddr, instanceAddr) == false)
				{
					diffs.Add(path + reflectionPropertyVariable->GetName(), reflectionPropertyVariable, sourceComponent, instanceComponent, sourceAddr, instanceAddr);
				}
			}
			else if (reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
			{
				ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(reflectionProperty);
				void* subObjectSourceAddr = reflectionPropertyObject->GetInstance(sourceAddr);
				void* subObjectInstanceAddr = reflectionPropertyObject->GetInstance(instanceAddr);
				std::string subObjecPath = path + reflectionPropertyObject->GetName() + ".";
				ReflectionDescriptor* subObjectReflectionDescriptor = reflectionPropertyObject->GetReflectionDescriptor();
				CollectDiff(sourceComponent, instanceComponent, diffs, subObjecPath, subObjectReflectionDescriptor, subObjectSourceAddr, subObjectInstanceAddr);
			}
		}
	}
}
