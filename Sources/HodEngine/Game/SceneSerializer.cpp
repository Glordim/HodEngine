#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <HodEngine/Core/Resource/ResourceManager.hpp>

namespace hod::game
{
	bool SceneSerializer::SerializeEntity(const std::shared_ptr<Entity> entity, bool withChildren, Document::Node& entitiesNode)
	{
		Document::Node& entityNode = entitiesNode.AddChild("");

		std::shared_ptr<PrefabResource> prefabResource = entity->GetPrefabResource();
		if (prefabResource != nullptr)
		{
			Document::Node& prefabInstance = entityNode.AddChild("PrefabInstance");
			UID uid = prefabResource->GetUid(); // TODO fix Serializa template lvalue
			Serializer::Serialize(uid, prefabInstance.AddChild("UID"));
			Document::Node& overridesNode = prefabInstance.AddChild("Overrides");

			PrefabUtility::EntityDiffs entityDiffs;
			PrefabUtility::CollectDiff(entity, entityDiffs);

			for (PrefabUtility::EntityDiffs::Diff* diff : entityDiffs._diffs)
			{
				Document::Node& overrideNode = overridesNode.AddChild("");
				Document::Node& overrideTargetNode = overrideNode.AddChild("Target");
				if (diff->_type == PrefabUtility::EntityDiffs::Diff::Type::Entity)
				{
					overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::EntityDiffs::Diff::Type::Entity));
					uid = static_cast<Entity*>(diff->_instance)->GetLocalId();
					Serializer::Serialize(uid, overrideTargetNode.AddChild("UID"));
				}
				else // Component
				{
					overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::EntityDiffs::Diff::Type::Component));
					uid = static_cast<Component*>(diff->_instance)->GetLocalId();
					Serializer::Serialize(uid, overrideTargetNode.AddChild("UID"));
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

			const std::vector<std::weak_ptr<Component>> components =  entity->GetComponents();
			for (const std::weak_ptr<Component>& component : components)
			{
				std::shared_ptr<Component> componentLock = component.lock();

				Document::Node& componentNode = componentsNode.AddChild("");
				componentNode.AddChild("MetaType").SetUInt64(componentLock->GetMetaType());
				Serializer::Serialize(componentLock.get(), componentNode);
			}

			if (withChildren)
			{
				uint32_t childCount = entity->GetChildCount();
				for (uint32_t childIndex = 0; childIndex < childCount; ++childIndex)
				{
					if (SceneSerializer::SerializeEntity(entity->GetChild(childIndex).Lock(), true, entitiesNode) == false)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	std::shared_ptr<Entity> SceneSerializer::InstantiateEntityFromDocumentNode(const Document::Node& entityNode, std::vector<std::shared_ptr<Entity>>& entities, std::vector<std::shared_ptr<Component>>& components)
	{
		const Document::Node* prefabInstanceNode = entityNode.GetChild("PrefabInstance");
		if (prefabInstanceNode != nullptr)
		{
			UID uid;
			Serializer::Deserialize(uid, *prefabInstanceNode->GetChild("UID"));

			std::shared_ptr<PrefabResource> prefabResource = ResourceManager::GetInstance()->GetResource<PrefabResource>(uid);
			//std::shared_ptr<Entity> entity = prefabResource->GetPrefab().GetRootEntity()->Clone(entities, components);
			std::shared_ptr<Entity> entity;

			Document prefabDocument;
			prefabResource->GetPrefab().GetRootEntity()->SetPrefabResource(nullptr);
			prefabResource->GetPrefab().SerializeInDocument(prefabDocument.GetRootNode());
			prefabResource->GetPrefab().GetRootEntity()->SetPrefabResource(prefabResource);
			const Document::Node* prefabEntitiesNode = prefabDocument.GetRootNode().GetChild("Entities");
			const Document::Node* prefabEntityNode = prefabEntitiesNode->GetFirstChild();
			while (prefabEntityNode != nullptr)
			{
				std::shared_ptr<Entity> prefabEntity = SceneSerializer::InstantiateEntityFromDocumentNode(*prefabEntityNode, entities, components);
				if (entity == nullptr) // todo store root id in prefab document ?
				{
					entity = prefabEntity;
					entity->SetPrefabResource(prefabResource);
				}
				prefabEntityNode = prefabEntityNode->GetNextSibling();
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
						const Document::Node* uidNode = targetNode->GetChild("UID");
						if (uidNode != nullptr)
						{
							UID localId;
							Serializer::Deserialize(localId, *uidNode);

							const Document::Node* modificationsNode = overrideNode->GetChild("Modification");
							if (modificationsNode != nullptr)
							{
								if (type == PrefabUtility::EntityDiffs::Diff::Type::Entity)
								{

								}
								else // Components
								{
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
								}
							}
						}
					}
				}

				// todo move in Instiate, here entity/component search has a heavy cost, searching during instiate will be better

				overrideNode = overrideNode->GetNextSibling();
			}

			return entity;
		}
		else
		{
			std::shared_ptr<Entity> entity = std::make_shared<Entity>("");
			ComponentFactory* componentFactory = ComponentFactory::GetInstance();

			Serializer::Deserialize(*entity.get(), entityNode);
			WeakEntityMapping::Insert(entity->GetId(), entity);

			const Document::Node* componentsNode = entityNode.GetChild("Components");
			const Document::Node* componentNode = componentsNode->GetFirstChild();
			while (componentNode != nullptr)
			{
				MetaType metaType = componentNode->GetChild("MetaType")->GetUInt64();

				auto it = componentFactory->GetAllDescriptors().find(metaType);
				if (it != componentFactory->GetAllDescriptors().end())
				{
					const ReflectionDescriptor& componentDescriptor = *it->second;
					std::weak_ptr<Component> component = entity->AddComponent(componentDescriptor, false);
					std::shared_ptr<Component> componentLock = component.lock();
					Component* rawComponent = componentLock.get();
					Serializer::Deserialize(rawComponent, *componentNode); // todo lvalue...
					WeakComponentMapping::Insert(componentLock->GetUid(), component);
					components.push_back(componentLock);
				}
				else
				{
					OUTPUT_ERROR("Unknown component !"); // TODO dangerous !!! user lost data
				}

				componentNode = componentNode->GetNextSibling();
			}

			entities.push_back(entity);
			return entity;
		}		
	}
}
