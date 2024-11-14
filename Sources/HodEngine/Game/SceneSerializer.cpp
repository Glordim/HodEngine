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
		ComponentFactory* componentFactory = ComponentFactory::GetInstance();

		std::shared_ptr<Entity> entity = std::make_shared<Entity>("");

		Serializer::Deserialize(*entity.get(), entityNode);
		WeakEntityMapping::Insert(entity->GetId(), entity);
		/*
		bool active = entityNode->GetChild("Active")->GetBool();
		const std::string& name = entityNode->GetChild("Name")->GetString();
		entityLock->SetActive(active);
		*/

		entities.push_back(entity);

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

		return entity;
	}
}
