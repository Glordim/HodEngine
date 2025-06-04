#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/World.hpp"

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
	Scene::Scene()
	{
		_world = nullptr;//World::GetInstance(); // TODO MultiWorld support
	}

	Scene::~Scene()
	{
		for (const auto& entityPair : _entities)
		{
			delete entityPair.second;
		}
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
				SceneSerializer sceneSerializer;
				if (sceneSerializer.SerializeEntity(entityPair.second, true, entitiesNode, _nextLocalId) == false)
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

		const Document::Node* entitiesNode = documentNode.GetChild("Entities");
		SceneSerializer sceneSerializer;
		sceneSerializer.Deserialize(*entitiesNode);

		for (Entity* entity : sceneSerializer.GetEntities())
		{
			_entities.emplace(entity->GetInstanceId(), entity);
			entity->SetScene(this);

			if (entity->GetParent().Lock() == nullptr)
			{
				entity->ProcessActivation();
			}
		}

		return true;
	}

	/// @brief 
	/// @return 
	const std::unordered_map<uint64_t, Entity*>& Scene::GetEntities() const
	{
		return _entities;
	}

	/// @brief 
	/// @param name 
	/// @return 
	Entity* Scene::CreateEntity(const std::string_view& name)
	{
		Entity* entity = new Entity(name);
		_entities.emplace(entity->GetInstanceId(), entity);

		entity->SetScene(this);

		entity->ProcessActivation();

		return entity;
	}

	/// @brief 
	/// @param entity 
	void Scene::DestroyEntity(Entity* entity)
	{
		auto it = _entities.find(entity->GetInstanceId());
		if (it != _entities.end())
		{
			_entities.erase(it);

			entity->Destruct();
		
			while (entity->GetChildren().empty() == false)
			{
				DestroyEntity(entity->GetChildren()[0].Lock());
			}

			entity->SetParent(WeakEntity(nullptr));
			entity->SetScene(nullptr);
		}
	}

	/// @brief 
	/// @param entityId 
	/// @return 
	Entity* Scene::FindEntity(uint64_t entityId)
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
		for (auto it : _entities)
		{
			it.second->SetScene(nullptr);
		}
		_entities.clear();
	}

	/// @brief 
	void Scene::ProcessActivation()
	{
		for (const auto& entityPair : _entities)
		{
			Entity* entity = entityPair.second;
			if (entity->GetParent().Lock() == nullptr)
			{
				entity->ProcessActivation();
			}
		}
	}

	/// @brief 
	void Scene::Update(float deltaTime)
	{
		for (auto entityPair : _entities)
		{
			for (Component* component : entityPair.second->GetComponents())
			{
				if (component->IsEnabledInHierarchy())
				{
					component->OnUpdate(deltaTime);
				}
			}
		}
	}

	/// @brief 
	void Scene::FixedUpdate()
	{
		for (auto entityPair : _entities)
		{
			for (Component* component : entityPair.second->GetComponents())
			{
				component->OnFixedUpdate();
			}
		}
	}

	/// @brief 
	/// @param renderQueue 
	void Scene::Draw(renderer::RenderQueue* renderQueue)
	{
		for (const auto& pair : _entities)
		{
			Entity* entity = pair.second;
			if (entity->IsActiveInHierarchy() == true)
			{
				RendererComponent* rendererComponent = entity->GetComponent<RendererComponent>();
				if (rendererComponent != nullptr)
				{
					rendererComponent->PushToRenderQueue(*renderQueue);
				}
			}
		}
	}

	/// @brief 
	/// @return 
	Scene* Scene::Clone(World* newWorld)
	{
		Scene* clone = new Scene();
		clone->SetWorld(newWorld);

		Document document;
		SerializeInDocument(document.GetRootNode());

		clone->DeserializeFromDocument(document.GetRootNode());
		return clone;
	}

	/// @brief 
	/// @param prefab 
	/// @return 
	Entity* Scene::Instantiate(std::shared_ptr<PrefabResource> prefabResource)
	{
		return Instantiate(prefabResource->GetPrefab().GetRootEntity());
	}

	/// @brief 
	/// @param entity 
	/// @return 
	Entity* Scene::Instantiate(Entity* entity)
	{
		if (entity == nullptr)
		{
			return nullptr;
		}

		Document document;
		SceneSerializer sceneSerializer;
		sceneSerializer.SerializeEntity(entity, true, document.GetRootNode(), _nextLocalId);

		sceneSerializer.Deserialize(document.GetRootNode());

		Entity* clonedEntity = nullptr;
		
		for (Entity* entity : sceneSerializer.GetEntities())
		{
			_entities.emplace(entity->GetInstanceId(), entity);
			entity->SetScene(this);
			entity->SetLocalId(0);
			for (Component* component : entity->GetComponents())
			{
				component->SetLocalId(0);
			}

			if (clonedEntity == nullptr && entity->GetParent().Lock() == nullptr)
			{
				clonedEntity = entity;
			}
		}
		
		clonedEntity->SetPrefabResource(entity->GetPrefabResource());
		clonedEntity->ProcessActivation();

		return clonedEntity;
	}

	/// @brief 
	/// @param world 
	void Scene::SetWorld(World* world)
	{
		_world = world;
	}

	/// @brief 
	/// @return 
	World* Scene::GetWorld() const
	{
		return _world;
	}
}
