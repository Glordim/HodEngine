#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"

#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output.hpp"
#include "HodEngine/Core/Color.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS_NO_PARENT(Prefab)
	{
		AddTrait<ReflectionTraitCustomSerialization>(
		[](const void* instance, Document::Node& documentNode)
		{
			static_cast<const Prefab*>(instance)->SerializeInDocument(documentNode);
		},
		[](void* instance, const Document::Node& documentNode)
		{
			static_cast<Prefab*>(instance)->DeserializeFromDocument(documentNode);
		});
	}

	/// @brief 
	/// @param uid 
	Prefab::Prefab(const UID& uid)
	: _uid(uid)
	{

	}

	/// @brief 
	/// @return 
	const UID& Prefab::GetUid() const
	{
		return _uid;
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
	bool Prefab::SerializeInDocument(Document::Node& documentNode) const
	{
		documentNode.AddChild("Name").SetString(_name);

		std::vector<std::shared_ptr<Entity>> entities;
		entities.reserve(_entities.size());
		for (const auto& entityPair : _entities)
		{
			entities.push_back(entityPair.second);
		}

		return SceneSerializer::SerializeEntities(entities, documentNode.AddChild("Entities"));
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

		ComponentFactory* componentFactory = ComponentFactory::GetInstance();

		const Document::Node* entitiesNode = documentNode.GetChild("Entities");
		const Document::Node* entityNode = entitiesNode->GetFirstChild();
		while (entityNode != nullptr)
		{
			std::weak_ptr<Entity> entity = CreateEntity();
			std::shared_ptr<Entity> entityLock = entity.lock();

			/*
			const std::string& name = entityNode->GetChild("Name")->GetString();
			bool active = entityNode->GetChild("Active")->GetBool();
			entityLock->SetActive(active);
			*/
			Serializer::Deserialize(*entity.lock().get(), *entityNode);

			const Document::Node* componentsNode = entityNode->GetChild("Components");
			const Document::Node* componentNode = componentsNode->GetFirstChild();
			while (componentNode != nullptr)
			{
				MetaType metaType = componentNode->GetChild("MetaType")->GetUInt64();

				auto it = componentFactory->GetAllDescriptors().find(metaType);
				if (it != componentFactory->GetAllDescriptors().end())
				{
					const ReflectionDescriptor& componentDescriptor = *it->second;
					std::weak_ptr<Component> component = entityLock->AddComponent(componentDescriptor, false);
					std::shared_ptr<Component> componentLock = component.lock();
					Component* rawComponent = componentLock.get();
					Serializer::Deserialize(rawComponent, *componentNode); // todo lvalue...
					rawComponent->SetLocalId(rawComponent->GetUid());
					WeakComponentMapping::Insert(componentLock->GetUid(), component);
				}
				else
				{
					OUTPUT_ERROR("Unknown component !"); // TODO dangerous !!! user lost data
				}

				componentNode = componentNode->GetNextSibling();
			}

			entityNode = entityNode->GetNextSibling();
		}

		return true;
	}

	/// @brief 
	/// @return 
	const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& Prefab::GetEntities() const
	{
		return _entities;
	}

	/// @brief 
	/// @param name 
	/// @return 
	std::weak_ptr<Entity> Prefab::CreateEntity(const std::string_view& name)
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(name);
		_entities.emplace(entity->GetId(), entity);

		return entity;
	}

	/// @brief 
	/// @param entity 
	void Prefab::DestroyEntity(std::shared_ptr<Entity> entity)
	{
		_entities.erase(_entities.find(entity->GetId()));
	}

	/// @brief 
	/// @param entityId 
	/// @return 
	std::weak_ptr<Entity> Prefab::FindEntity(Entity::Id entityId)
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
	Prefab* Prefab::Clone()
	{
		Prefab* clone = new Prefab();

		std::map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> sourceToCloneEntitiesMap;		
		std::map<std::shared_ptr<Component>, std::shared_ptr<Component>> sourceToCloneComponentsMap;

		clone->_entities.reserve(_entities.size());
		for (const auto& entityPair : _entities)
		{
			std::shared_ptr<Entity> cloneEntity = entityPair.second->Clone();
			sourceToCloneEntitiesMap.emplace(entityPair.second, cloneEntity);

			for (size_t componentIndex = 0; componentIndex < cloneEntity->GetComponents().size(); ++componentIndex)
			{
				sourceToCloneComponentsMap.emplace(entityPair.second->GetComponents()[componentIndex], cloneEntity->GetComponents()[componentIndex]);
			}

			clone->_entities.emplace(cloneEntity->GetId(), cloneEntity);
		}
			
		for (const auto& componentPair : sourceToCloneComponentsMap)
		{
			ReflectionDescriptor* reflectionDescriptor = componentPair.second->GetReflectionDescriptorV();
			for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
			{
				switch (reflectionProperty->GetMetaType())
				{
					case ReflectionPropertyArray::GetMetaTypeStatic():
					{
						//ReflectionPropertyArray* reflectionPropertyArray = static_cast<ReflectionPropertyArray*>(reflectionProperty);
					}
					break;

					case ReflectionPropertyObject::GetMetaTypeStatic():
					{
						//ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(reflectionProperty);

						//if (reflectionPropertyObject->GetReflectionDescriptor() == WeakComponent::)
					}
					break;
				}
			}
		}

		return clone;
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
