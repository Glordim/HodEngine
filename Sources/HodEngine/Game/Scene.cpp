#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <algorithm>
#include <unordered_map>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(Scene)
	{
		HOD_TRAIT(ReflectionTraitCustomSerialization,
			[](const void* instance, DocumentNode& documentNode) { return const_cast<Scene*>(static_cast<const Scene*>(instance))->SerializeInDocument(documentNode); },
			[](void* instance, const DocumentNode& documentNode) { return static_cast<Scene*>(instance)->DeserializeFromDocument(documentNode); });
	}

	/// @brief
	Scene::Scene()
	{
		_world = nullptr; // World::GetInstance(); // TODO MultiWorld support
	}

	Scene::~Scene()
	{
		DestroyEntities();
	}

	namespace
	{
		/// @brief The roots among the entities of entitiesNode (written by SceneSerializer::SerializeEntity), in the document's order:
		/// the deserialized entities come unordered, the roots are found back in the document by their local id
		Vector<Entity*> OrderRootsLikeDocument(const DocumentNode& entitiesNode, const Vector<Entity*>& unorderedRoots)
		{
			Vector<Entity*> roots;
			for (const DocumentNode* entityNode = entitiesNode.GetFirstChild(); entityNode != nullptr; entityNode = entityNode->GetNextSibling())
			{
				const DocumentNode* localIdNode = entityNode->GetChild("_localId");
				if (localIdNode == nullptr)
				{
					continue;
				}
				uint64_t localId = localIdNode->GetUInt64();
				for (Entity* root : unorderedRoots)
				{
					if (root->GetLocalId() == localId && std::find(roots.Begin(), roots.End(), root) == roots.End())
					{
						roots.push_back(root);
						break;
					}
				}
			}
			for (Entity* root : unorderedRoots)
			{
				if (std::find(roots.Begin(), roots.End(), root) == roots.End())
				{
					roots.push_back(root);
				}
			}
			return roots;
		}
	}

	/// @brief
	void Scene::DestroyEntities()
	{
		_rootEntities.Clear(); // Not kept up to date while deleted (their parents may be deleted first)

		for (const auto& entityPair : _entities)
		{
			if (entityPair.second->GetParent().Lock() == nullptr)
			{
				entityPair.second->Destruct();
			}
		}

		for (const auto& entityPair : _entities)
		{
			DefaultAllocator::GetInstance().Delete(entityPair.second);
		}
		_entities.clear();
	}

	/// @brief
	/// @return
	Vector<Entity*> Scene::GetRootEntities() const
	{
		return _rootEntities;
	}

	/// @brief
	/// @param entity
	void Scene::AddRoot(Entity* entity)
	{
		if (std::find(_rootEntities.Begin(), _rootEntities.End(), entity) == _rootEntities.End())
		{
			_rootEntities.PushBack(entity);
		}
	}

	/// @brief
	/// @param entity
	void Scene::RemoveRoot(Entity* entity)
	{
		auto it = std::find(_rootEntities.Begin(), _rootEntities.End(), entity);
		if (it != _rootEntities.End())
		{
			_rootEntities.Erase(it);
		}
	}

	/// @brief
	/// @param entity
	/// @return
	uint32_t Scene::GetRootIndex(const Entity* entity) const
	{
		for (uint32_t index = 0; index < _rootEntities.Size(); ++index)
		{
			if (_rootEntities[index] == entity)
			{
				return index;
			}
		}
		return 0;
	}

	/// @brief Same as Entity::SetSiblingIndex, among the roots
	/// @param entity
	/// @param index
	void Scene::SetRootIndex(Entity* entity, uint32_t index)
	{
		auto it = std::find(_rootEntities.Begin(), _rootEntities.End(), entity);
		if (it == _rootEntities.End())
		{
			return;
		}
		_rootEntities.Erase(it);
		index = Min(index, static_cast<uint32_t>(_rootEntities.Size()));
		_rootEntities.Insert(_rootEntities.Begin() + index, entity);
	}

	/// @brief
	/// @param roots
	void Scene::MoveRootsToEnd(const Vector<Entity*>& roots)
	{
		for (Entity* root : roots)
		{
			RemoveRoot(root);
		}
		for (Entity* root : roots)
		{
			_rootEntities.PushBack(root);
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
	const String& Scene::GetName() const
	{
		return _name;
	}

	/// @brief
	/// @param documentNode
	bool Scene::SerializeInDocument(DocumentNode& documentNode)
	{
		documentNode.AddChild("Name").SetString(_name);
		DocumentNode& entitiesNode = documentNode.AddChild("Entities");

		// The same state written the same way (the editor's undo compares them): the roots in their order, given their ids first
		Vector<Entity*> roots = GetRootEntities();
		for (Entity* root : roots)
		{
			if (root->GetLocalId() == 0)
			{
				root->SetLocalId(_nextLocalId++);
			}
		}

		for (Entity* root : roots)
		{
			SceneSerializer sceneSerializer;
			if (sceneSerializer.SerializeEntity(root, true, entitiesNode, _nextLocalId) == false)
			{
				return false;
			}
		}

		documentNode.AddChild("NextLocalId").SetUInt64(_nextLocalId);

		return true;
	}

	/// @brief
	/// @param documentNode
	bool Scene::DeserializeFromDocument(const DocumentNode& documentNode)
	{
		const DocumentNode* nameNode = documentNode.GetChild("Name");
		if (nameNode != nullptr)
		{
			_name = nameNode->GetString();
		}

		const DocumentNode* nextLocalIdNode = documentNode.GetChild("NextLocalId");
		if (nextLocalIdNode != nullptr)
		{
			_nextLocalId = nextLocalIdNode->GetUInt64();
		}

		const DocumentNode* entitiesNode = documentNode.GetChild("Entities");
		SceneSerializer       sceneSerializer;
		if (sceneSerializer.Deserialize(*entitiesNode) == false)
		{
			return false;
		}

		Vector<Entity*> unorderedRoots;
		for (Entity* entity : sceneSerializer.GetEntities())
		{
			_entities.emplace(entity->GetInstanceId(), entity);
			entity->SetScene(this);
			if (entity->GetParent().Lock() == nullptr)
			{
				unorderedRoots.PushBack(entity);
			}
		}
		MoveRootsToEnd(OrderRootsLikeDocument(*entitiesNode, unorderedRoots));

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
		Entity* entity = DefaultAllocator::GetInstance().New<Entity>(name);
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

			while (entity->GetChildren().Empty() == false)
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
		_rootEntities.Clear();
	}

	/// @brief
	void Scene::ProcessActivation()
	{
		for (const auto& entityPair : _entities)
		{
			Entity* entity = entityPair.second;
			if (entity->GetParent().Lock() == nullptr)
			{
				entity->Construct();
			}
		}

		for (const auto& entityPair : _entities)
		{
			Entity* entity = entityPair.second;
			if (entity->GetParent().Lock() == nullptr)
			{
				if (entity->GetActive())
				{
					if (entity->GetScene()->GetWorld()->GetEditorPlaying())
					{
						entity->Awake();
					}
					entity->Enable();
				}
				else if (entity->IsActiveInHierarchy())
				{
					entity->Disable();
				}
			}
		}

		for (const auto& entityPair : _entities)
		{
			Entity* entity = entityPair.second;
			if (entity->GetParent().Lock() == nullptr && entity->GetActive())
			{
				if (entity->GetScene()->GetWorld()->GetEditorPlaying())
				{
					entity->Start();
				}
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
	/// @return
	Scene* Scene::Clone(World* newWorld)
	{
		Scene* clone = DefaultAllocator::GetInstance().New<Scene>();
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
		Entity* entity = Instantiate(prefabResource->GetPrefab().GetRootEntity());
		entity->SetPrefabResource(prefabResource);
		entity->SetLocalId(0);
		return entity;
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

		Document        document;
		SceneSerializer sceneSerializer;
		sceneSerializer.SerializeEntity(entity, true, document.GetRootNode(), _nextLocalId);

		sceneSerializer.Deserialize(document.GetRootNode());

		Entity* clonedEntity = nullptr;

		for (Entity* entity : sceneSerializer.GetEntities())
		{
			_entities.emplace(entity->GetInstanceId(), entity);
			entity->SetScene(this);

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
	/// @param entitiesNode
	/// @return
	Vector<Entity*> Scene::Instantiate(const DocumentNode& entitiesNode)
	{
		Vector<Entity*> roots;

		SceneSerializer sceneSerializer;
		if (sceneSerializer.Deserialize(entitiesNode) == false)
		{
			return roots;
		}

		Vector<Entity*> unorderedRoots;
		for (Entity* entity : sceneSerializer.GetEntities())
		{
			_entities.emplace(entity->GetInstanceId(), entity);
			entity->SetScene(this);
			if (entity->GetParent().Lock() == nullptr)
			{
				unorderedRoots.push_back(entity);
			}
		}

		roots = OrderRootsLikeDocument(entitiesNode, unorderedRoots);
		MoveRootsToEnd(roots);

		for (Entity* root : roots)
		{
			root->ProcessActivation();
		}
		return roots;
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
