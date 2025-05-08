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
	bool CollectPrefabOverrideInternalRecursive(std::shared_ptr<Entity> source, std::shared_ptr<Entity> instance, const std::vector<uint64_t>& target, std::vector<PrefabOverride>& overrides);
	void CollectEntityOverride(std::shared_ptr<Entity> sourceEntity, std::shared_ptr<Entity> instanceEntity, std::vector<PrefabOverride>& overrides, const std::vector<uint64_t>& target, const std::string& path, ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr);
	void CollectComponentOverride(std::shared_ptr<Component> sourceComponent, std::shared_ptr<Component> instanceComponent, std::vector<PrefabOverride>& overrides, const std::vector<uint64_t>& target, const std::string& path, ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr);

	/// @brief 
	/// @param entity 
	/// @param overrides 
	/// @return 
	bool CollectPrefabOverride(std::shared_ptr<Entity> entity, std::vector<PrefabOverride>& overrides)
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

		std::vector<uint64_t> target;
		return CollectPrefabOverrideInternalRecursive(source, instance, target, overrides);
	}
	
	/// @brief 
	/// @param entity 
	/// @param overrides 
	/// @return 
	bool CollectPrefabOverrideInternalRecursive(std::shared_ptr<Entity> source, std::shared_ptr<Entity> instance, const std::vector<uint64_t>& target, std::vector<PrefabOverride>& overrides)
	{
		std::vector<uint64_t> entityTarget = target;
		entityTarget.push_back(source->GetLocalId());
		CollectEntityOverride(source, instance, overrides, entityTarget, "", Entity::GetReflectionDescriptor(), source.get(), instance.get());
		
		for (std::weak_ptr<Component> component : source->GetComponents())
		{
			std::shared_ptr<Component> componentLock = component.lock();
			
			uint64_t localId = componentLock->GetLocalId();
			for (std::weak_ptr<Component> instanceComponent : instance->GetComponents())
			{
				std::shared_ptr<Component> instanceComponentLock = instanceComponent.lock();
				if (instanceComponentLock->GetLocalId() == localId)
				{
					std::vector<uint64_t> componentTarget = target;
					componentTarget.push_back(instanceComponentLock->GetLocalId());
					
					ReflectionDescriptor& reflectionDescriptor = componentLock->GetReflectionDescriptorV();
					CollectComponentOverride(componentLock, instanceComponentLock, overrides, componentTarget, "", reflectionDescriptor, componentLock.get(), instanceComponentLock.get());
				}
			}
		}

		for (const WeakEntity& sourceChild : source->GetChildren())
		{
			std::shared_ptr<hod::game::Entity> sourceChildEntity = sourceChild.Lock();
			std::shared_ptr<hod::game::Entity> instanceChildEntity = nullptr;
			for (const WeakEntity& instanceChild : instance->GetChildren())
			{
				std::shared_ptr<hod::game::Entity> instanceChildLock = instanceChild.Lock();
				if (instanceChildLock->GetLocalId() == sourceChildEntity->GetLocalId())
				{
					instanceChildEntity = instanceChildLock;
					break;
				}
			}

			if (instanceChildEntity != nullptr)
			{
				CollectPrefabOverrideInternalRecursive(sourceChildEntity, instanceChildEntity, target, overrides);
			}
		}

		return true;
	}

	/// @brief 
	/// @param sourceEntity 
	/// @param instanceEntity 
	/// @param overrides 
	/// @param path 
	/// @param reflectionDescriptor 
	/// @param sourceAddr 
	/// @param instanceAddr 
	void CollectEntityOverride(std::shared_ptr<Entity> sourceEntity, std::shared_ptr<Entity> instanceEntity, std::vector<PrefabOverride>& overrides, const std::vector<uint64_t>& target, const std::string& path, ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr)
	{
		for (ReflectionProperty* reflectionProperty : reflectionDescriptor.GetProperties())
		{
			if (reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
			{
				ReflectionPropertyVariable* reflectionPropertyVariable = static_cast<ReflectionPropertyVariable*>(reflectionProperty);
				if (reflectionPropertyVariable->CompareInstance(sourceAddr, instanceAddr) == false)
				{
					PrefabOverride override;
					override._type = PrefabOverride::Type::Entity;
					override._source = sourceEntity.get();
					override._instance = instanceEntity.get();
					override._reflectionProperty = reflectionPropertyVariable;
					override._target = target;
					override._path = path + reflectionPropertyVariable->GetName();
					override._effectiveSourceAddr = sourceAddr;
					override._effectiveInstanceAddr = instanceAddr;
					overrides.push_back(override);
				}
			}
			else if (reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
			{
				ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(reflectionProperty);
				void* subObjectSourceAddr = reflectionPropertyObject->GetInstance(sourceAddr);
				void* subObjectInstanceAddr = reflectionPropertyObject->GetInstance(instanceAddr);
				std::string subObjecPath = path + reflectionPropertyObject->GetName() + ".";
				ReflectionDescriptor* subObjectReflectionDescriptor = reflectionPropertyObject->GetReflectionDescriptor();
				CollectEntityOverride(sourceEntity, instanceEntity, overrides, target, subObjecPath, *subObjectReflectionDescriptor, subObjectSourceAddr, subObjectInstanceAddr);
			}
		}
	}

	/// @brief 
	/// @param sourceComponent 
	/// @param instanceComponent 
	/// @param overrides 
	/// @param path 
	/// @param reflectionDescriptor 
	/// @param sourceAddr 
	/// @param instanceAddr 
	void CollectComponentOverride(std::shared_ptr<Component> sourceComponent, std::shared_ptr<Component> instanceComponent, std::vector<PrefabOverride>& overrides, const std::vector<uint64_t>& target, const std::string& path, ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr)
	{
		if (reflectionDescriptor.GetParent() != nullptr)
		{
			CollectComponentOverride(sourceComponent, instanceComponent, overrides, target, path, *reflectionDescriptor.GetParent(), sourceAddr, instanceAddr);
		}

		for (ReflectionProperty* reflectionProperty : reflectionDescriptor.GetProperties())
		{
			if (reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
			{
				ReflectionPropertyVariable* reflectionPropertyVariable = static_cast<ReflectionPropertyVariable*>(reflectionProperty);
				if (reflectionPropertyVariable->CompareInstance(sourceAddr, instanceAddr) == false)
				{
					PrefabOverride override;
					override._type = PrefabOverride::Type::Component;
					override._source = sourceComponent.get();
					override._instance = instanceComponent.get();
					override._reflectionProperty = reflectionPropertyVariable;
					override._target = target;
					override._path = path + reflectionPropertyVariable->GetName();
					override._effectiveSourceAddr = sourceAddr;
					override._effectiveInstanceAddr = instanceAddr;
					overrides.push_back(override);
				}
			}
			else if (reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
			{
				ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(reflectionProperty);
				void* subObjectSourceAddr = reflectionPropertyObject->GetInstance(sourceAddr);
				void* subObjectInstanceAddr = reflectionPropertyObject->GetInstance(instanceAddr);
				std::string subObjecPath = path + reflectionPropertyObject->GetName() + ".";
				ReflectionDescriptor* subObjectReflectionDescriptor = reflectionPropertyObject->GetReflectionDescriptor();
				CollectComponentOverride(sourceComponent, instanceComponent, overrides, target, subObjecPath, *subObjectReflectionDescriptor, subObjectSourceAddr, subObjectInstanceAddr);
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

			for (const WeakEntity& weakChild : parent->GetChildren())
			{
				std::shared_ptr<game::Entity> child = weakChild.Lock();
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
		std::shared_ptr<game::Entity> entity = component->GetOwner();
		if (entity != nullptr)
		{
			std::shared_ptr<game::Entity> prefabInstanceEntity = GetPrefabInstance(entity);
			if (prefabInstanceEntity != nullptr)
			{
				std::string pathToComponent = GetRelativePath(prefabInstanceEntity, entity);
				std::shared_ptr<Entity> correspondingEntity = FindChildByPath(prefabInstanceEntity->GetPrefabResource()->GetPrefab().GetRootEntity(), pathToComponent);
				if (correspondingEntity != nullptr)
				{
					return correspondingEntity->GetComponent(component->GetReflectionDescriptorV());
				}
			}
		}
		return nullptr;
	}
}
