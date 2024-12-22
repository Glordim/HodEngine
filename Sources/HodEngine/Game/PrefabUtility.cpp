#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Color.hpp"

namespace hod::game::PrefabUtility
{
	/// @brief 
	/// @param entity 
	/// @param diffs 
	/// @return 
	bool CollectDiff(std::shared_ptr<Entity> entity, EntityDiffs& diffs)
	{
		std::shared_ptr<PrefabResource> prefabResource = entity->GetPrefabResource();
		if (prefabResource == nullptr)
		{
			return false;
		}

		std::shared_ptr<Entity> source = prefabResource->GetPrefab().GetRootEntity();
		std::shared_ptr<Entity> instance = entity;

		if (source == instance)
		{
			return true;
		}

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

	/// @brief 
	/// @param entity 
	/// @return 
	std::shared_ptr<Entity> GetPrefabInstance(std::shared_ptr<Entity> entity)
	{
		while (entity != nullptr)
		{
			std::shared_ptr<PrefabResource> prefabResource = entity->GetPrefabResource();
			if (prefabResource != nullptr)
			{
				return entity;
			}
			else
			{
				entity = entity->GetParent().Lock();
			}
		}
		return nullptr;
	}

	/// @brief 
	/// @param parent 
	/// @param child 
	/// @return 
	std::string GetRelativePath(std::shared_ptr<Entity> parent, std::shared_ptr<Entity> child)
	{
		std::string relativePath;
		while (child != nullptr && child != parent)
		{
			if (relativePath.empty() == false)
			{
				relativePath.insert('/', child->GetName());
			}
			relativePath.insert(0, child->GetName());

			child = child->GetParent().Lock();
		}
		return relativePath;
	}

	/// @brief 
	/// @param parent 
	/// @param relativePath 
	/// @return 
	std::shared_ptr<Entity> FindChildByPath(std::shared_ptr<Entity> parent, std::string_view relativePath)
	{
		size_t offset = 0;
		while (offset < relativePath.size())
		{
			size_t separatorPos = relativePath.find('/', offset);
			std::string_view name(relativePath.data() + offset, separatorPos);

			bool childFound = false;

			uint32_t childCount = parent->GetChildCount();
			for (uint32_t childIndex = 0; childIndex < childCount; ++childIndex)
			{
				std::shared_ptr<game::Entity> child = parent->GetChild(childIndex).Lock();
				if (child->GetName() == name)
				{
					parent = child;
					offset += separatorPos + 1;
					childFound = true;
					break;
				}
			}
			if (childFound == false)
			{
				return nullptr;
			}
		}
		return parent;
	}

	/// @brief 
	/// @param sourceComponent 
	/// @return 
	std::shared_ptr<Component> GetCorrespondingComponent(std::shared_ptr<Component> component)
	{
		std::shared_ptr<game::Entity> entity = component->GetEntity();
		if (entity != nullptr)
		{
			std::shared_ptr<game::Entity> prefabInstanceEntity = GetPrefabInstance(entity);
			if (prefabInstanceEntity != nullptr)
			{
				std::string pathToComponent = GetRelativePath(prefabInstanceEntity, entity);
				std::shared_ptr<Entity> correspondingEntity = FindChildByPath(prefabInstanceEntity->GetPrefabResource()->GetPrefab().GetRootEntity(), pathToComponent);
				if (correspondingEntity != nullptr)
				{
					return correspondingEntity->GetComponent(component->GetMetaType());
				}
			}
		}
		return nullptr;
	}
}
