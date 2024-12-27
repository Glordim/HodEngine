#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"

#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Color.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(Prefab, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
		[](const void* instance, Document::Node& documentNode)
		{
			const_cast<Prefab*>(static_cast<const Prefab*>(instance))->SerializeInDocument(documentNode);
		},
		[](void* instance, const Document::Node& documentNode)
		{
			static_cast<Prefab*>(instance)->DeserializeFromDocument(documentNode);
		});
	}

	/// @brief 
	/// @param name 
	void Prefab::SetName(const std::string_view& name)
	{
		_name = name;
	}

	/// @brief 
	/// @return 
	const std::string& Prefab::GetName() const
	{
		return _name;
	}

	/// @brief 
	/// @param documentNode 
	bool Prefab::SerializeInDocument(Document::Node& documentNode)
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
	bool Prefab::DeserializeFromDocument(const Document::Node& documentNode)
	{
		const Document::Node* nameNode = documentNode.GetChild("Name");
		if (nameNode!= nullptr)
		{
			_name = nameNode->GetString();
		}

		const Document::Node* nextLocalIdNode = documentNode.GetChild("NextLocalId");
		if (nextLocalIdNode != nullptr)
		{
			_nextLocalId = nextLocalIdNode->GetUInt64();
		}

		ComponentFactory* componentFactory = ComponentFactory::GetInstance();

		const Document::Node* entitiesNode = documentNode.GetChild("Entities");
		const Document::Node* entityNode = entitiesNode->GetFirstChild();
		while (entityNode != nullptr)
		{
			std::vector<std::shared_ptr<Entity>> entities;
			std::vector<std::shared_ptr<Component>> components;
			std::shared_ptr<Entity> entity = SceneSerializer::InstantiateEntityFromDocumentNode(*entityNode, entities, components);
			_entities.emplace(entity->GetInstanceId(), entity);

			entityNode = entityNode->GetNextSibling();
		}

		return true;
	}

	/// @brief 
	/// @return 
	const std::unordered_map<uint64_t, std::shared_ptr<Entity>>& Prefab::GetEntities() const
	{
		return _entities;
	}

	/// @brief 
	/// @param name 
	/// @return 
	std::weak_ptr<Entity> Prefab::CreateEntity(const std::string_view& name)
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(name);
		WeakEntityMapping::Insert(entity->GetInstanceId(), entity);
		_entities.emplace(entity->GetInstanceId(), entity);

		return entity;
	}

	/// @brief 
	/// @param entity 
	void Prefab::DestroyEntity(std::shared_ptr<Entity> entity)
	{
		_entities.erase(_entities.find(entity->GetInstanceId()));
	}

	/// @brief 
	/// @param entityId 
	/// @return 
	std::weak_ptr<Entity> Prefab::FindEntity(uint64_t entityId)
	{
		return _entities[entityId];
	}

	/// @brief 
	void Prefab::Clear()
	{
		_entities.clear();
	}

	/// @brief 
	/// @return 
	std::shared_ptr<Entity> Prefab::GetRootEntity()
	{
		if (_entities.empty())
		{
			return nullptr;
		}
		return _entities.begin()->second;
	}
}
