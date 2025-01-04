#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/Entity.hpp"

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

#include "HodEngine/Core/Resource/ResourceManager.hpp"

#include <unordered_map>
#include <vector>

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(Scene, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
		[](const void* instance, Document::Node& documentNode)
		{
			const_cast<Scene*>(static_cast<const Scene*>(instance))->SerializeInDocument(documentNode);
		},
		[](void* instance, const Document::Node& documentNode)
		{
			static_cast<Scene*>(instance)->DeserializeFromDocument(documentNode);
		});
	}

	/// @brief 
	/// @param name 
	void Scene::SetName(const std::string_view& name)
	{
		_name = name;
	}

	/// @brief 
	/// @return 
	const std::string& Scene::GetName() const
	{
		return _name;
	}

	/// @brief 
	/// @param documentNode 
	bool Scene::SerializeInDocument(Document::Node& documentNode)
	{
		documentNode.AddChild("Name").SetString(_name);
		Document::Node& entitiesNode = documentNode.AddChild("Entities");

		for (const auto& entityPair : _entities)
		{
			if (entityPair.second->GetParent().Lock() == nullptr)
			{
				if (SceneSerializer::SerializeEntity(entityPair.second, true, entitiesNode, _nextLocalId) == false)
				{
					return false;
				}
			}
		}

		documentNode.AddChild("NextLocalId").SetUInt64(_nextLocalId);

		return true;
	}

	/// @brief 
	/// @param documentNode 
	bool Scene::DeserializeFromDocument(const Document::Node& documentNode)
	{
		const Document::Node* nameNode = documentNode.GetChild("Name");
		if (nameNode != nullptr)
		{
			_name = nameNode->GetString();
		}

		const Document::Node* nextLocalIdNode = documentNode.GetChild("NextLocalId");
		if (nextLocalIdNode != nullptr)
		{
			_nextLocalId = nextLocalIdNode->GetUInt64();
		}

		std::unordered_map<uint64_t, std::shared_ptr<Entity>> entities;
		std::unordered_map<uint64_t, std::shared_ptr<Component>> components;
		const Document::Node* entitiesNode = documentNode.GetChild("Entities");
		const Document::Node* entityNode = entitiesNode->GetFirstChild();
		while (entityNode != nullptr)
		{
			SceneSerializer::InstantiateEntityFromDocumentNode(*entityNode, entities, components);

			entityNode = entityNode->GetNextSibling();
		}

		for (auto& entityPair : entities)
		{
			uint64_t parentId = entityPair.second->GetParent().GetInstanceId();
			if (parentId != 0)
			{
				entityPair.second->SetParent(entities[parentId]);
			}
		}
		for (auto& componentPair : components)
		{
			std::vector<WeakEntity*> weakEntities;
			componentPair.second->GetReflectionDescriptorV()->CollectObjectProperties(weakEntities, componentPair.second.get());
			for (WeakEntity* weakEntity : weakEntities)
			{
				uint64_t entityId = weakEntity->GetInstanceId();
				if (entityId != 0)
				{
					weakEntity->SetPointer(entities[entityId]);
				}
			}

			std::vector<WeakComponentBase*> weakComponents;
			componentPair.second->GetReflectionDescriptorV()->CollectObjectProperties(weakComponents, componentPair.second.get());
			for (WeakComponentBase* weakComponent : weakComponents)
			{
				uint64_t componenId = weakComponent->GetInstanceId();
				if (componenId != 0)
				{
					weakComponent->SetPointer(components[componenId]);
				}
			}
		}

		for (auto& entityPair : entities)
		{
			_entities.emplace(entityPair.second->GetInstanceId(), entityPair.second);
		}

		for (const auto& entity : _entities)
		{
			for (std::weak_ptr<Component> component : entity.second->GetComponents())
			{
				component.lock()->Construct();
			}
		}

		for (const auto& entity : _entities)
		{
			entity.second->SetActive(true);
		}

		return true;
	}

	/// @brief 
	/// @return 
	const std::unordered_map<uint64_t, std::shared_ptr<Entity>>& Scene::GetEntities() const
	{
		return _entities;
	}

	/// @brief 
	/// @param name 
	/// @return 
	std::shared_ptr<Entity> Scene::CreateEntity(const std::string_view& name)
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(name);
		_entities.emplace(entity->GetInstanceId(), entity);

		return entity;
	}

	/// @brief 
	/// @param entity 
	void Scene::DestroyEntity(std::shared_ptr<Entity> entity)
	{
		auto it = _entities.find(entity->GetInstanceId());
		if (it != _entities.end())
		{
			_entities.erase(it);
		}
	}

	/// @brief 
	/// @param entityId 
	/// @return 
	std::shared_ptr<Entity> Scene::FindEntity(uint64_t entityId)
	{
		auto it = _entities.find(entityId);
		if (it != _entities.end())
		{
			return it->second;
		}
		return nullptr;
	}

	/// @brief 
	void Scene::Clear()
	{
		_entities.clear();
	}

	/// @brief 
	void Scene::Awake()
	{
		for (const auto& entity : _entities)
		{
			entity.second->Awake();
		}
	}

	///
	void Scene::Start()
	{
		for (const auto& entity : _entities)
		{
			entity.second->Start();
		}
	}

	/// @brief 
	void Scene::Update()
	{
		for (auto entityPair : _entities)
		{
			for (std::weak_ptr<Component> component : entityPair.second->GetComponents())
			{
				component.lock()->OnUpdate();
			}
		}
	}

	/// @brief 
	/// @param renderQueue 
	void Scene::Draw(renderer::RenderQueue* renderQueue)
	{
		for (const auto& pair : _entities)
		{
			std::shared_ptr<Entity> entity = pair.second;
			std::shared_ptr<RendererComponent> rendererComponent = entity->GetComponent<RendererComponent>();
			if (rendererComponent != nullptr)
			{
				rendererComponent->PushToRenderQueue(*renderQueue);
			}
		}
	}

	/// @brief 
	/// @return 
	Scene* Scene::Clone()
	{
		Scene* clone = new Scene();

		Document document;
		SerializeInDocument(document.GetRootNode());

		clone->DeserializeFromDocument(document.GetRootNode());
		return clone;
	}

	/// @brief 
	/// @param prefab 
	/// @return 
	std::shared_ptr<Entity> Scene::Instantiate(std::shared_ptr<PrefabResource> prefabResource)
	{
		return Instantiate(prefabResource->GetPrefab().GetRootEntity());
	}

	/// @brief 
	/// @param entity 
	/// @return 
	std::shared_ptr<Entity> Scene::Instantiate(std::shared_ptr<Entity> entity)
	{
		if (entity == nullptr)
		{
			return nullptr;
		}

		uint64_t nextLocalId = 1; // todo
		Document document;
		SceneSerializer::SerializeEntity(entity, true, document.GetRootNode(), nextLocalId);

		std::unordered_map<uint64_t, std::shared_ptr<Entity>> entities;
		std::unordered_map<uint64_t, std::shared_ptr<Component>> components;

		std::shared_ptr<Entity> clonedEntity;

		const Document::Node* entityNode = document.GetRootNode().GetFirstChild();
		while (entityNode != nullptr)
		{
			std::shared_ptr<Entity> newEntity = SceneSerializer::InstantiateEntityFromDocumentNode(*entityNode, entities, components);
			if (clonedEntity == nullptr)
			{
				clonedEntity = newEntity;
				clonedEntity->SetPrefabResource(entity->GetPrefabResource());
			}
			entityNode = entityNode->GetNextSibling();
		}

		for (auto& entityPair : entities)
		{
			_entities.emplace(entityPair.second->GetInstanceId(), entityPair.second);
		}

		return clonedEntity;
	}
}
