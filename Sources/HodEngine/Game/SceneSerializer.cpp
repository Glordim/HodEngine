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
	bool SceneSerializer::SerializeEntity(const std::shared_ptr<Entity> entity, bool withChildren, Document::Node& entitiesNode, uint64_t& nextLocalId)
	{
		Document::Node& entityNode = entitiesNode.AddChild("");

		if (entity->GetLocalId() == 0)
		{
			entity->SetLocalId(nextLocalId);
			++nextLocalId;
		}

		std::shared_ptr<PrefabResource> prefabResource = entity->GetPrefabResource();
		if (prefabResource != nullptr)
		{
			entityNode.AddChild("_localId").SetUInt64(entity->GetLocalId());

			Document::Node& prefabInstance = entityNode.AddChild("PrefabInstance");
			UID uid = prefabResource->GetUid(); // TODO fix Serializa template lvalue
			Serializer::Serialize(uid, prefabInstance.AddChild("UID"));
			Document::Node& overridesNode = prefabInstance.AddChild("Overrides");

			PrefabUtility::EntityDiffs entityDiffs;
			PrefabUtility::CollectDiff(entity, entityDiffs);

			for (PrefabUtility::EntityDiffs::Diff* diff : entityDiffs._diffs)
			{
				if (diff->_reflectionProperty->FindTrait<ReflectionTraitNoSerialization>() != nullptr)
				{
					continue;
				}

				if (diff->_type == PrefabUtility::EntityDiffs::Diff::Type::Entity && std::strcmp(diff->_reflectionProperty->GetName(), "_localId") == 0) // toto better way
				{
					continue;
				}

				Document::Node& overrideNode = overridesNode.AddChild("");
				Document::Node& overrideTargetNode = overrideNode.AddChild("Target");
				if (diff->_type == PrefabUtility::EntityDiffs::Diff::Type::Entity)
				{
					overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::EntityDiffs::Diff::Type::Entity));
					overrideTargetNode.AddChild("LocalId").SetUInt64(static_cast<Entity*>(diff->_instance)->GetLocalId());
				}
				else // Component
				{
					overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::EntityDiffs::Diff::Type::Component));
					overrideTargetNode.AddChild("LocalId").SetUInt64(static_cast<Component*>(diff->_instance)->GetLocalId());
				}
				// TODO can be an array of modifcations to mutalize target description
				Document::Node& overrideModificationNode = overrideNode.AddChild("Modification");
				Serializer::SerializeVariable((ReflectionPropertyVariable*)diff->_reflectionProperty, diff->_effectiveInstanceAddr, overrideModificationNode, diff->_path);
			}
		}
		else
		{
			Serializer::Serialize(entity.get(), entityNode);

			Document::Node& componentsNode = entityNode.AddChild("Components");

			for (std::shared_ptr<Component> component : entity->GetComponents())
			{
				Document::Node& componentNode = componentsNode.AddChild("");
				componentNode.AddChild("MetaType").SetUInt64(component->GetMetaType());
				if (component->GetLocalId() == 0)
				{
					component->SetLocalId(nextLocalId);
					++nextLocalId;
				}
				Serializer::Serialize(component.get(), componentNode);
			}

			if (withChildren)
			{
				for (const WeakEntity& child : entity->GetChildren())
				{
					if (SceneSerializer::SerializeEntity(child.Lock(), true, entitiesNode, nextLocalId) == false)
					{
						return false;
					}
				}
			}
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
			std::vector<WeakEntity*> weakEntities;
			componentPair.second->GetReflectionDescriptorV().CollectObjectProperties(weakEntities, componentPair.second.get());
			for (WeakEntity* weakEntity : weakEntities)
			{
				uint64_t entityId = weakEntity->GetInstanceId();
				if (entityId != 0)
				{
					weakEntity->SetPointer(_contextualEntityMap[entityId]);
				}
			}

			std::vector<WeakComponentBase*> weakComponents;
			componentPair.second->GetReflectionDescriptorV().CollectObjectProperties(weakComponents, componentPair.second.get());
			for (WeakComponentBase* weakComponent : weakComponents)
			{
				uint64_t componenId = weakComponent->GetInstanceId();
				if (componenId != 0)
				{
					weakComponent->SetPointer(_contextualComponentMap[componenId]);
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
		std::shared_ptr<Entity> entity = std::make_shared<Entity>("");
		ComponentFactory* componentFactory = ComponentFactory::GetInstance();

		Serializer::Deserialize(*entity.get(), entityNode);

		const Document::Node* componentsNode = entityNode.GetChild("Components");
		const Document::Node* componentNode = componentsNode->GetFirstChild();
		while (componentNode != nullptr)
		{
			MetaType metaType = componentNode->GetChild("MetaType")->GetUInt64();

			auto it = componentFactory->GetAllDescriptors().find(metaType);
			if (it != componentFactory->GetAllDescriptors().end())
			{
				const ReflectionDescriptor& componentDescriptor = *it->second;
				std::shared_ptr<Component> component = entity->AddComponent(componentDescriptor);
				Serializer::Deserialize(componentDescriptor, component.get(), *componentNode); // todo lvalue...

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
			if (targetNode != nullptr)
			{
				const Document::Node* typeNode = targetNode->GetChild("Type");
				if (targetNode != nullptr)
				{
					PrefabUtility::EntityDiffs::Diff::Type type = static_cast<PrefabUtility::EntityDiffs::Diff::Type>(typeNode->GetInt64());
					uint64_t localId = targetNode->GetChild("LocalId")->GetUInt64();
					
					const Document::Node* modificationsNode = overrideNode->GetChild("Modification");
					if (modificationsNode != nullptr)
					{
						if (type == PrefabUtility::EntityDiffs::Diff::Type::Entity)
						{

						}
						else // Components
						{
							/*
							std::shared_ptr<Component> targetComponent;
							for (const std::shared_ptr<Component>& component : components)
							{
								if (component->GetLocalId() == localId)
								{
									targetComponent = component;
									break;
								}
							}
							if (targetComponent != nullptr)
							{
								Component* component = targetComponent.get();
								const Document::Node* modificationNode = modificationsNode->GetFirstChild();
								while (modificationNode != nullptr)
								{
									Serializer::DeserializeWithPath(modificationNode->GetName(), component, *modificationsNode);
									modificationNode = modificationNode->GetNextSibling();
								}
							}
							else
							{
								// todo
							}
							*/
						}
					}
				}
			}

			// todo move in Instiate, here entity/component search has a heavy cost, searching during instiate will be better

			overrideNode = overrideNode->GetNextSibling();
		}

		std::shared_ptr<Entity> rootEntity = sceneSerializer._totalEntities[0];
		rootEntity->SetPrefabResource(prefabResource);
		rootEntity->SetLocalId(entityNode.GetChild("_localId")->GetUInt64());

		if (_contextualEntityMap.try_emplace(rootEntity->GetLocalId(), rootEntity).second == false)
		{
			OUTPUT_ERROR("SceneSerializer::DeserializeEntityPrefab: LocalId already exist !");
			return false;
		}

		for (std::shared_ptr<Entity> entity : sceneSerializer._totalEntities)
		{
			_totalEntities.push_back(entity);
		}

		for (std::shared_ptr<Component> component : sceneSerializer._totalComponent)
		{
			_totalComponent.push_back(component);
		}

		return true;
	}

	/// @brief 
	/// @return 
	const std::vector<std::shared_ptr<Entity>>& SceneSerializer::GetEntities() const
	{
		return _totalEntities;
	}

	/// @brief 
	/// @return 
	const std::vector<std::shared_ptr<Component>>& SceneSerializer::GetComponents() const
	{
		return _totalComponent;
	}
}
