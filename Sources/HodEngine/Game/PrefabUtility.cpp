#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Core/Color.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::game::PrefabUtility
{
	bool CollectPrefabOverrideInternalRecursive(Entity* source, Entity* instance, const Vector<uint64_t>& target, Vector<PrefabOverride>& overrides);
	void CollectEntityOverride(Entity* sourceEntity, Entity* instanceEntity, Vector<PrefabOverride>& overrides, const Vector<uint64_t>& target, const String& path,
	                           ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr);
	void CollectComponentOverride(Component* sourceComponent, Component* instanceComponent, Vector<PrefabOverride>& overrides, const Vector<uint64_t>& target, const String& path,
	                              ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr);

	/// @brief
	/// @param entity
	/// @param overrides
	/// @return
	bool CollectPrefabOverride(Entity* entity, Vector<PrefabOverride>& overrides)
	{
		std::shared_ptr<PrefabResource> prefabResource = entity->GetPrefabResource();
		if (prefabResource == nullptr)
		{
			return false;
		}

		Entity* source = prefabResource->GetPrefab().GetRootEntity();
		Entity* instance = entity;

		if (source == instance)
		{
			return true;
		}

		Vector<uint64_t> target;
		return CollectPrefabOverrideInternalRecursive(source, instance, target, overrides);
	}

	/// @brief
	/// @param entity
	/// @param overrides
	/// @return
	bool CollectPrefabOverrideInternalRecursive(Entity* source, Entity* instance, const Vector<uint64_t>& target, Vector<PrefabOverride>& overrides)
	{
		Vector<uint64_t> entityTarget = target;
		entityTarget.push_back(source->GetLocalId());
		CollectEntityOverride(source, instance, overrides, entityTarget, "", Entity::GetReflectionDescriptor(), source, instance);

		for (Component* component : source->GetComponents())
		{
			uint64_t localId = component->GetLocalId();
			for (Component* instanceComponent : instance->GetComponents())
			{
				if (instanceComponent->GetLocalId() == localId)
				{
					Vector<uint64_t> componentTarget = target;
					componentTarget.push_back(instanceComponent->GetLocalId());

					ReflectionDescriptor& reflectionDescriptor = component->GetReflectionDescriptorV();
					CollectComponentOverride(component, instanceComponent, overrides, componentTarget, "", reflectionDescriptor, component, instanceComponent);
				}
			}
		}

		for (const WeakEntity& sourceChild : source->GetChildren())
		{
			Entity* sourceChildEntity = sourceChild.Lock();
			Entity* instanceChildEntity = nullptr;
			for (const WeakEntity& instanceChild : instance->GetChildren())
			{
				Entity* instanceChildLock = instanceChild.Lock();
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
	void CollectEntityOverride(Entity* sourceEntity, Entity* instanceEntity, Vector<PrefabOverride>& overrides, const Vector<uint64_t>& target, const String& path,
	                           ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr)
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
					override._source = sourceEntity;
					override._instance = instanceEntity;
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
				void*                     subObjectSourceAddr = reflectionPropertyObject->GetInstance(sourceAddr);
				void*                     subObjectInstanceAddr = reflectionPropertyObject->GetInstance(instanceAddr);
				String                    subObjecPath = path + reflectionPropertyObject->GetName() + ".";
				ReflectionDescriptor*     subObjectReflectionDescriptor = reflectionPropertyObject->GetReflectionDescriptor();
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
	void CollectComponentOverride(Component* sourceComponent, Component* instanceComponent, Vector<PrefabOverride>& overrides, const Vector<uint64_t>& target, const String& path,
	                              ReflectionDescriptor& reflectionDescriptor, void* sourceAddr, void* instanceAddr)
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
					override._source = sourceComponent;
					override._instance = instanceComponent;
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
				void*                     subObjectSourceAddr = reflectionPropertyObject->GetInstance(sourceAddr);
				void*                     subObjectInstanceAddr = reflectionPropertyObject->GetInstance(instanceAddr);
				String                    subObjecPath = path + reflectionPropertyObject->GetName() + ".";
				ReflectionDescriptor*     subObjectReflectionDescriptor = reflectionPropertyObject->GetReflectionDescriptor();
				CollectComponentOverride(sourceComponent, instanceComponent, overrides, target, subObjecPath, *subObjectReflectionDescriptor, subObjectSourceAddr,
				                         subObjectInstanceAddr);
			}
		}
	}

	/// @brief
	/// @param entity
	/// @return
	Entity* GetPrefabInstance(Entity* entity)
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
	String GetRelativePath(Entity* parent, Entity* child)
	{
		String relativePath;
		while (child != nullptr && child != parent)
		{
			if (relativePath.Empty() == false)
			{
				relativePath.Insert(0, "/");
			}
			relativePath.Insert(0, child->GetName());

			child = child->GetParent().Lock();
		}
		return relativePath;
	}

	/// @brief
	/// @param parent
	/// @param relativePath
	/// @return
	Entity* FindChildByPath(Entity* parent, std::string_view relativePath)
	{
		size_t offset = 0;
		while (offset < relativePath.size())
		{
			size_t           separatorPos = relativePath.find('/', offset);
			std::string_view name(relativePath.data() + offset, separatorPos);

			bool childFound = false;

			for (const WeakEntity& weakChild : parent->GetChildren())
			{
				Entity* child = weakChild.Lock();
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
	Component* GetCorrespondingComponent(Component* component)
	{
		game::Entity* entity = component->GetOwner();
		if (entity != nullptr)
		{
			game::Entity* prefabInstanceEntity = GetPrefabInstance(entity);
			if (prefabInstanceEntity != nullptr)
			{
				String  pathToComponent = GetRelativePath(prefabInstanceEntity, entity);
				Entity* correspondingEntity = FindChildByPath(prefabInstanceEntity->GetPrefabResource()->GetPrefab().GetRootEntity(), pathToComponent);
				if (correspondingEntity != nullptr)
				{
					return correspondingEntity->GetComponent(component->GetReflectionDescriptorV());
				}
			}
		}
		return nullptr;
	}
}
