#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/WeakEntity.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitNoSerialization.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <HodEngine/Core/Resource/ResourceManager.hpp>

#include <cstring>

namespace hod::game
{
	/// @brief 
	/// @param entity 
	/// @param withChildren 
	/// @param entityNode 
	/// @param nextLocalId 
	/// @return 
	bool SceneSerializer::SerializeEntity(Entity* entity, bool withChildren, Document::Node& entitiesNode, uint64_t& nextLocalId)
	{
		if (entity->GetPrefabResource() != nullptr)
		{
			return SerializeEntityPrefab(entity, entitiesNode, nextLocalId);
		}
		else
		{
			return SerializeEntityRaw(entity, entitiesNode, nextLocalId);
		}
	}

	/// @brief 
	/// @param entityNode 
	/// @param entityNode 
	/// @param nextLocalId 
	/// @return 
	bool SceneSerializer::SerializeEntityRaw(Entity* entity, Document::Node& entitiesNode, uint64_t& nextLocalId)
	{
		Document::Node& entityNode = entitiesNode.AddChild("");

		if (entity->GetLocalId() == 0)
		{
			entity->SetLocalId(nextLocalId);
			++nextLocalId;
		}

		Serializer::Serialize(entity, entityNode);

		Document::Node& componentsNode = entityNode.AddChild("Components");

		for (Component* component : entity->GetComponents())
		{
			Document::Node& componentNode = componentsNode.AddChild("");
			componentNode.AddChild("MetaType").SetUInt64(component->GetMetaType());
			if (component->GetLocalId() == 0)
			{
				component->SetLocalId(nextLocalId);
				++nextLocalId;
			}
			if (Serializer::Serialize(component, componentNode, std::bind(&SceneSerializer::CustomComponentSerializationCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)) == false)
			{
				return false;
			}
		}

		for (const WeakEntity& child : entity->GetChildren())
		{
			if (SceneSerializer::SerializeEntity(child.Lock(), true, entitiesNode, nextLocalId) == false)
			{
				return false;
			}
		}

		return true;
	}

	/// @brief 
	/// @param entity 
	/// @param entityNode 
	/// @param nextLocalId 
	/// @return 
	bool SceneSerializer::SerializeEntityPrefab(Entity* entity, Document::Node& entitiesNode, uint64_t& nextLocalId)
	{
		Document::Node& entityNode = entitiesNode.AddChild("");

		entityNode.AddChild("_localId").SetUInt64(entity->GetLocalId());

		Document::Node& prefabInstance = entityNode.AddChild("PrefabInstance");
		UID uid = entity->GetPrefabResource()->GetUid(); // TODO fix Serializa template lvalue
		Serializer::Serialize(uid, prefabInstance.AddChild("UID"));
		Document::Node& overridesNode = prefabInstance.AddChild("Overrides");

		Vector<PrefabUtility::PrefabOverride> prefabOverrides;
		PrefabUtility::CollectPrefabOverride(entity, prefabOverrides);

		for (const PrefabUtility::PrefabOverride& override : prefabOverrides)
		{
			if (override._reflectionProperty->FindTrait<ReflectionTraitNoSerialization>() != nullptr)
			{
				continue;
			}

			if (override._type == PrefabUtility::PrefabOverride::Type::Entity && std::strcmp(override._reflectionProperty->GetName(), "_localId") == 0) // toto better way
			{
				continue;
			}

			Document::Node& overrideNode = overridesNode.AddChild("");
			Document::Node& overrideTargetNode = overrideNode.AddChild("Target");
			if (override._type == PrefabUtility::PrefabOverride::Type::Entity)
			{
				overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::PrefabOverride::Type::Entity));
			}
			else // Component
			{
				overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::PrefabOverride::Type::Component));
			}

			overrideTargetNode.AddChild("LocalIds").SetValues(std::span(override._target));

			// TODO can be an array of modifcations to mutalize target description
			Document::Node& overrideModificationNode = overrideNode.AddChild("Modification");
			Serializer::SerializeVariable((ReflectionPropertyVariable*)override._reflectionProperty, override._effectiveInstanceAddr, overrideModificationNode, override._path);
		}

		return true;
	}

	/// @brief 
	/// @param entitiesNode 
	/// @return 
	bool SceneSerializer::Deserialize(const Document::Node& entitiesNode)
	{
		const Document::Node* entityNode = entitiesNode.GetFirstChild();
		while (entityNode != nullptr)
		{
			if (DeserializeEntity(*entityNode) == false)
			{
				return false;
			}
			entityNode = entityNode->GetNextSibling();
		}

		for (auto& entityPair : _contextualEntityMap)
		{
			uint64_t parentId = entityPair.second->GetParent().GetInstanceId();
			if (parentId != 0)
			{
				entityPair.second->SetParent(_contextualEntityMap[parentId]);
			}

			_totalEntities.push_back(entityPair.second);
		}
		for (auto& componentPair : _contextualComponentMap)
		{
			Vector<WeakEntity*> weakEntities;
			componentPair.second->GetReflectionDescriptorV().CollectObjectProperties(weakEntities, componentPair.second);
			for (WeakEntity* weakEntity : weakEntities)
			{
				uint64_t entityId = weakEntity->GetInstanceId();
				if (entityId != 0)
				{
					weakEntity->SetPointer(_contextualEntityMap[entityId]);
				}
			}

			for (const std::pair<WeakComponentBase*, Vector<EntityComponentRoute>>& weakComponentToResolve : _weakComponentsToResolve)
			{
				const Vector<EntityComponentRoute>& routes = weakComponentToResolve.second;
				Entity* entity = nullptr;
				SceneSerializer* sceneSerializer = this;
				for (uint32_t i = 0; i < routes.size(); ++i)
				{
					auto it = sceneSerializer->_contextualEntityMap.find(routes[i]._entityId);
					if (it != sceneSerializer->_contextualEntityMap.end())
					{
						entity = it->second;
						std::shared_ptr<PrefabResource> prefabResource = entity->GetPrefabResource();
						if (prefabResource != nullptr)
						{
							sceneSerializer = nullptr;
						}
						else
						{
							// todo error ?
						}
					}
					else
					{
						// todo output error or better ? missing target can interest users
					}
				}

				if (entity != nullptr)
				{
					uint64_t componentId = routes.back()._componentId;
					for (Component* component : entity->GetComponents())
					{
						if (component->GetLocalId() == componentId)
						{
							weakComponentToResolve.first->SetPointer(component);
							break;
						}
					}
				}
			}

			_totalComponent.push_back(componentPair.second);
		}

		return true;
	}

	/// @brief 
	/// @param entityNode 
	/// @return 
	bool SceneSerializer::DeserializeEntity(const Document::Node& entityNode)
	{
		const Document::Node* prefabInstanceNode = entityNode.GetChild("PrefabInstance");
		if (prefabInstanceNode != nullptr)
		{
			return DeserializeEntityPrefab(entityNode);
		}
		else
		{
			return DeserializeEntityRaw(entityNode);
		}
	}

	/// @brief 
	/// @param entityNode 
	/// @return 
	bool SceneSerializer::DeserializeEntityRaw(const Document::Node& entityNode)
	{
		Entity* entity = DefaultAllocator::GetInstance().New<Entity>("");
		ComponentFactory* componentFactory = ComponentFactory::GetInstance();

		Serializer::Deserialize(*entity, entityNode);

		const Document::Node* componentsNode = entityNode.GetChild("Components");
		const Document::Node* componentNode = componentsNode->GetFirstChild();
		while (componentNode != nullptr)
		{
			MetaType metaType = componentNode->GetChild("MetaType")->GetUInt64();

			auto it = componentFactory->GetAllDescriptors().find(metaType);
			if (it != componentFactory->GetAllDescriptors().end())
			{
				const ReflectionDescriptor& componentDescriptor = *it->second;
				Component* component = entity->AddComponent(componentDescriptor);
				if (Serializer::Deserialize(componentDescriptor, component, *componentNode, std::bind(&SceneSerializer::CustomComponentDeserializationCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)) == false) // todo lvalue...
				{
					return false;
				}

				if (_contextualComponentMap.try_emplace(component->GetLocalId(), component).second == false)
				{
					OUTPUT_ERROR("SceneSerializer::DeserializeEntityRaw: LocalId already exist !");
					return false;
				}
			}
			else
			{
				OUTPUT_ERROR("SceneSerializer::DeserializeEntityRaw: Unknown component !"); // TODO dangerous !!! user lost data
				return false;
			}

			componentNode = componentNode->GetNextSibling();
		}

		if (_contextualEntityMap.try_emplace(entity->GetLocalId(), entity).second == false)
		{
			OUTPUT_ERROR("SceneSerializer::DeserializeEntityRaw: LocalId already exist !");
			return false;
		}

		return true;
	}

	/// @brief 
	/// @param entityNode 
	/// @return 
	bool SceneSerializer::DeserializeEntityPrefab(const Document::Node& entityNode)
	{
		const Document::Node* prefabInstanceNode = entityNode.GetChild("PrefabInstance");
		if (prefabInstanceNode == nullptr)
		{
			assert(false);
			return false;
		}

		UID uid;
		Serializer::Deserialize(uid, *prefabInstanceNode->GetChild("UID"));

		std::shared_ptr<PrefabResource> prefabResource = ResourceManager::GetInstance()->GetResource<PrefabResource>(uid);
		const Document::Node* prefabEntitiesNode = prefabResource->GetDocument().GetRootNode().GetChild("Entities");

		SceneSerializer sceneSerializer;
		if (sceneSerializer.Deserialize(*prefabEntitiesNode) == false)
		{
			return false;
		}

		const Document::Node* overridesNode = prefabInstanceNode->GetChild("Overrides");
		const Document::Node* overrideNode = overridesNode->GetFirstChild();
		while (overrideNode != nullptr)
		{
			const Document::Node* targetNode = overrideNode->GetChild("Target");
			const Document::Node* modificationsNode = overrideNode->GetChild("Modification");
			if (targetNode != nullptr && modificationsNode != nullptr)
			{
				const Document::Node* typeNode = targetNode->GetChild("Type");
				const Document::Node* localIdsNode = targetNode->GetChild("LocalIds");
				if (typeNode != nullptr && localIdsNode != nullptr)
				{
					PrefabUtility::PrefabOverride::Type type = static_cast<PrefabUtility::PrefabOverride::Type>(typeNode->GetInt64());

					const Document::Node* localIdNode = localIdsNode->GetFirstChild();
					uint64_t localId = localIdNode->GetUInt64();

					if (type == PrefabUtility::PrefabOverride::Type::Entity)
					{
						auto it = sceneSerializer._contextualEntityMap.find(localId);
						if (it != sceneSerializer._contextualEntityMap.end())
						{
							Entity* entity = it->second;
							Serializer::Deserialize(*entity, *modificationsNode);
						}
					}
					else // Components
					{
						auto it = sceneSerializer._contextualComponentMap.find(localId);
						if (it != sceneSerializer._contextualComponentMap.end())
						{
							Component* component = it->second;
							const Document::Node* modificationNode = modificationsNode->GetFirstChild();
							while (modificationNode != nullptr)
							{
								Serializer::DeserializeWithPath(modificationNode->GetName(), *component, *modificationsNode);
								modificationNode = modificationNode->GetNextSibling();
							}
							//Serializer::Deserialize(*component.get(), *modificationsNode);
						}
					}
				}
			}

			// todo move in Instiate, here entity/component search has a heavy cost, searching during instiate will be better

			overrideNode = overrideNode->GetNextSibling();
		}

		Entity* rootEntity = nullptr;

		for (Entity* entity : sceneSerializer._totalEntities)
		{
			if (rootEntity == nullptr && entity->GetParent().Lock() == nullptr)
			{
				rootEntity = entity;
			}
			else
			{
				_totalEntities.push_back(entity);
			}
		}

		for (Component* component : sceneSerializer._totalComponent)
		{
			_totalComponent.push_back(component);
		}

		rootEntity->SetPrefabResource(prefabResource);
		rootEntity->SetLocalId(entityNode.GetChild("_localId")->GetUInt64());
		if (_contextualEntityMap.try_emplace(rootEntity->GetLocalId(), rootEntity).second == false)
		{
			OUTPUT_ERROR("SceneSerializer::DeserializeEntityPrefab: LocalId already exist !");
			return false;
		}

		return true;
	}

	/// @brief 
	/// @return 
	const Vector<Entity*>& SceneSerializer::GetEntities() const
	{
		return _totalEntities;
	}

	/// @brief 
	/// @return 
	const Vector<Component*>& SceneSerializer::GetComponents() const
	{
		return _totalComponent;
	}

	/// @brief 
	/// @param instance 
	/// @param reflectionDescriptor 
	/// @param documentNode 
	bool SceneSerializer::CustomComponentSerializationCallback(const void* instance, const ReflectionDescriptor& reflectionDescriptor, Document::Node& documentNode)
	{
		if (WeakComponentBase::GetReflectionDescriptor().IsCompatible(reflectionDescriptor))
		{
			const WeakComponentBase* weakComponent = static_cast<const WeakComponentBase*>(instance);
			Component* component = weakComponent->Lock();
			if (component != nullptr)
			{
				Entity* entity = component->GetOwner();

				Vector<EntityComponentRoute> routes;
				EntityComponentRoute route;
				route._entityId = entity->GetLocalId();
				route._componentId = component->GetLocalId();
				routes.push_back(route);

				while (entity != nullptr)
				{
					std::shared_ptr<PrefabResource> prefabResource = entity->GetPrefabResource();
					if (prefabResource != nullptr && entity->GetLocalId() != routes.back()._entityId)
					{
						EntityComponentRoute route;
						route._entityId = entity->GetLocalId();
						route._componentId = 0;
						routes.push_back(route);
					}
					entity = entity->GetParent().Lock();
				}

				for (auto it = routes.rbegin(); it != routes.rend(); ++it)
				{
					Document::Node& routeNode = documentNode.AddChild("");
					routeNode.AddChild("Entity").SetUInt64(it->_entityId);
					routeNode.AddChild("Component").SetUInt64(it->_componentId);
				}
			}
		}

		return true;
	}

	/// @brief 
	/// @param instance 
	/// @param reflectionDescriptor 
	/// @param documentNode 
	bool SceneSerializer::CustomComponentDeserializationCallback(void* instance, const ReflectionDescriptor& reflectionDescriptor, const Document::Node& documentNode)
	{
		if (WeakComponentBase::GetReflectionDescriptor().IsCompatible(reflectionDescriptor))
		{
			WeakComponentBase* weakComponent = static_cast<WeakComponentBase*>(instance);
			Vector<EntityComponentRoute> routes;
			const Document::Node* childNode = documentNode.GetFirstChild();
			while (childNode != nullptr)
			{
				EntityComponentRoute route;

				const Document::Node* entityNode = childNode->GetChild("Entity");
				if (entityNode)
				{
					route._entityId = entityNode->GetUInt64();
				}
				else
				{
					// todo format error
				}

				const Document::Node* componentNode = childNode->GetChild("Component");
				if (componentNode)
				{
					route._componentId = componentNode->GetUInt64();
				}
				else
				{
					// todo format error
				}
				routes.push_back(route);
				childNode = childNode->GetNextSibling();
			}
			_weakComponentsToResolve.emplace_back(weakComponent, std::move(routes));
		}

		return true;
	}
}
