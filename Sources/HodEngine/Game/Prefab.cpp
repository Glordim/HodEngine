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
	bool Prefab::DeserializeFromDocument(const Document::Node& documentNode)
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
			if (_rootEntity == nullptr && entity->GetParent().Lock() == nullptr)
			{
				_rootEntity = entity;
			}
		}

		return true;
	}

	/// @brief 
	/// @return 
	const std::unordered_map<uint64_t, Entity*>& Prefab::GetEntities() const
	{
		return _entities;
	}

	/// @brief 
	/// @param name 
	/// @return 
	Entity* Prefab::CreateEntity(const std::string_view& name)
	{
		Entity* entity = new Entity(name);
		_entities.emplace(entity->GetInstanceId(), entity);

		return entity;
	}

	/// @brief 
	/// @param entity 
	void Prefab::DestroyEntity(Entity* entity)
	{
		_entities.erase(_entities.find(entity->GetInstanceId()));
	}

	/// @brief 
	/// @param entityId 
	/// @return 
	Entity* Prefab::FindEntity(uint64_t entityId)
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
	Entity* Prefab::GetRootEntity()
	{
		return _rootEntity;
	}
}
